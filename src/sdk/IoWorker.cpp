/*
 Copyright 2024 Huawei Technologies Co.,Ltd.

 Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements.  See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership.  The ASF licenses this file
 to you under the Apache LICENSE, Version 2.0 (the
 "LICENSE"); you may not use this file except in compliance
 with the LICENSE.  You may obtain a copy of the LICENSE at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing,
 software distributed under the LICENSE is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied.  See the LICENSE for the
 specific language governing permissions and limitations
 under the LICENSE.
*/

#include "IoWorker.h"
#include "util.h"
#include "logger.h"
#include <cmath>

IoWorker::IoWorker() {
    sem_init(&sem, 0, 10);
    retryQueueShutDownFlag.store(false);
}

IoWorker::~IoWorker() {
}

void IoWorker::Init(Client *client, RetryQueue *queue, Config *cfg, Producer *producer) {
    sem_init(&sem, 0, cfg->MaxIoWorkers);

    this->client = client;
    this->retryQueue = queue;
    this->producer = producer;
    this->taskCount = 0;

    for (auto elem: cfg->NoRetryCodeList) {
        this->noRetryStatusCodeMap[elem] = nullptr;
    }
}

void IoWorker::sendToServer(Batch *batch) {
    int64_t beginMs = GetTimeMs(UnixNano());

    HttpResponse response;
    this->client->PutLogs(batch->getGroupId(), batch->getStreamId(), batch->logGroup, response);
    if (response.HttpCode == 200) {
        this->processSendSuccess(batch, beginMs);
    } else {
        this->processSendFailed(batch, beginMs, response);
    }
}

void IoWorker::processSendSuccess(Batch* batch, int64_t beginMs) {
    this->producer->producerLogGroupSize -= batch->totalDataSize;

    if (batch->attemptCount < batch->maxReservedAttempts) {
        int64_t nowMS = GetTimeMs(UnixNano());
        Attempt* attempt = new Attempt();
        attempt->Init(true, "", "", "", nowMS, nowMS-beginMs, 200);
        batch->result->attemptList.push_back(attempt);
    }

    batch->result->successful = true;

    for (auto &d: batch->callBackList) {
        if (d.Success != nullptr) {
            d.Success(batch->result);
        }
    }

    delete batch;
}

void IoWorker::addErrorMessageToBatchAttempt(Batch* batch, HttpResponse& response, bool retryInfo, int64_t beginMs) {
    if (batch->attemptCount < batch->maxReservedAttempts) {
        int64_t nowMs = GetTimeMs(UnixNano());

        Attempt* attempt = new Attempt();
        attempt->Init(false, response.requestId,
                      response.errorCode, response.errorMessage,
                      nowMs, nowMs - beginMs,
                      response.HttpCode);

        batch->result->attemptList.push_back(attempt);
    }

    batch->result->successful = false;
    batch->attemptCount++;
}

void IoWorker::processSendFailed(Batch* batch, int64_t beginMs, HttpResponse &response) {
    if (this->retryQueueShutDownFlag.load()) {
        this->producer->producerLogGroupSize -= batch->totalDataSize;
        this->addErrorMessageToBatchAttempt(batch, response, false, beginMs);
        for (auto &d: batch->callBackList) {
            if (d.Fail != nullptr) {
                d.Fail(batch->result);
            }
        }

        delete batch;

        LogPrint(LOG_LEVEL_ERROR, "processSendFailed() drop logs[%d]", batch->logGroup->logs.size());

        return;
    }

    if (batch->attemptCount < batch->maxRetryTimes) {
        this->addErrorMessageToBatchAttempt(batch, response, true, beginMs);

        int64_t retryWaitTime = batch->baseRetryBackoffMs +
                int64_t(std::pow(2, batch->attemptCount-1));

        batch->nextRetryMs = GetTimeMs(UnixNano()) +
                    std::min(retryWaitTime, batch->maxRetryIntervalInMs);

        this->retryQueue->sendToRetryQueue(batch);
    } else {
        this->producer->producerLogGroupSize -= batch->totalDataSize;
        for (auto &d: batch->callBackList) {
            if (d.Fail != nullptr) {
                d.Fail(batch->result);
            }
        }

        LogPrint(LOG_LEVEL_ERROR, "processSendFailed() drop logs[%d]", batch->logGroup->logs.size());

        delete batch;
    }
}

void IoWorker::CloseRetryQueue() {
    this->retryQueueShutDownFlag.store(true);
}

void IoWorker::allocSem() {
    sem_wait(&sem);
}

void IoWorker::freeSem() {
    sem_post(&sem);
}

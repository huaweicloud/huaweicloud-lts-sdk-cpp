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

#include "Mover.h"
#include "util.h"

Mover::Mover() {
    this->moverShutDownFlag.store(false);
    this->threadCount = 0;
}

Mover::~Mover() {
}

void Mover::Init(LogAccumulator *logAccumulator, RetryQueue *retryQueue, IoWorker *ioWorker, IoThreadPool *ioThreadPool, Config* config) {
    this->logAccumulator = logAccumulator;
    this->retryQueue = retryQueue;
    this->ioWorker = ioWorker;
    this->ioThreadPool = ioThreadPool;
    this->config = config;
}

void Mover::Run() {
    addThreadCount();

    for (;this->moverShutDownFlag.load() != true;) {
        this->processBatch();

        if (this->moverShutDownFlag.load()) {
            break;
        }
    }

    this->processLeftBatch();

    subThreadCount();
}

void Mover::processBatch() {
    int64_t sleepMs = this->config->LingerMs;
    int64_t nowTimeMs = GetTimeMs(UnixNano());

    this->logAccumulator->Lock();

    std::map<std::string, Batch*> logGroupData = this->logAccumulator->GetLogGroupData();
    int count = logGroupData.size();

    for (auto &d: logGroupData) {
        std::string key = d.first;
        Batch* batch = d.second;

        int64_t timeInterval = batch->createTimeMs + this->config->LingerMs - nowTimeMs;
        if (timeInterval <= 0) {
            this->sendToServer(key, batch);
        } else {
            if (sleepMs > timeInterval) {
                sleepMs = timeInterval;
            }
        }
    }

    this->logAccumulator->UnLock();

    if (count == 0) {
        sleepMs = this->config->LingerMs;
    }

    std::vector<Batch*> batchList = this->retryQueue->getRetryBatch(this->moverShutDownFlag.load());
    if (batchList.size() == 0) {
        SleepMs(sleepMs);
    } else {
        for (auto &batch: batchList) {
            this->ioThreadPool->addTask(batch);
        }
    }
}

void Mover::processLeftBatch() {
    this->logAccumulator->Lock();

    auto logGroupData = this->logAccumulator->GetLogGroupData();
    for (auto &data: logGroupData) {
        Batch* batch = data.second;
        this->ioThreadPool->addTask(batch);
    }

    this->logAccumulator->clearLogGroupData();

    this->logAccumulator->UnLock();

    std::vector<Batch*> batchList = this->retryQueue->getRetryBatch(this->moverShutDownFlag.load());
    for (auto &batch: batchList) {
        this->ioThreadPool->addTask(batch);
    }
}

void Mover::sendToServer(std::string key, Batch *batch) {
    Batch *value = this->logAccumulator->find4LogGroupData(key);
    if (value == nullptr) {
        return;
    }

    if (GetTimeMs(UnixNano()) - value->createTimeMs < this->config->LingerMs) {
        return;
    }

    this->ioThreadPool->addTask(batch);

    this->logAccumulator->delete4LogGroupData(key);
}

void Mover::Close() {
    this->moverShutDownFlag.store(true);
    while (!isThreadCountZero()) {
        SleepMs(500);
    }
}

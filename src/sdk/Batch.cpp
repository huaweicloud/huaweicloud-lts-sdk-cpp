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

#include "Batch.h"
#include "util.h"

Batch::Batch() {
    this->nextRetryMs = 0;
}

Batch::~Batch() {
    if (result != nullptr) {
        while (!result->attemptList.empty()) {
            Attempt* attempt = result->attemptList.back();
            result->attemptList.pop_back();
            delete attempt;
        }

        delete result;
    }
    if (logGroup != nullptr) {
        delete logGroup;
    }
}

void Batch::Init(std::vector<Log *> log, CallBack *cb, std::string groupId, std::string streamId, Config *config) {
    this->logGroup = new LogGroup();
    this->logGroup->Init(log);

    this->nextRetryMs = 0;
    this->createTimeMs = GetTimeMs(UnixNano());
    this->attemptCount = 0;
    this->maxRetryIntervalInMs = config->MaxRetryBackoffMs;
    this->callBackList = std::vector<CallBack>();
    this->maxRetryTimes = config->Retries;
    this->baseRetryBackoffMs = config->BaseRetryBackoffMs;
    this->groupId = groupId;
    this->streamId = streamId;
    this->result = new Result();
    this->maxReservedAttempts = config->MaxReservedAttempts;
    this->totalDataSize = this->logGroup->Size();

    if (cb != nullptr) {
        this->callBackList.push_back(*cb);
    }
}
bool Batch::operator > (const Batch *b) const {
    return this->nextRetryMs > b->nextRetryMs;
}

std::string Batch::getGroupId() {
    mtx.lock();
    std::string groupId = this->groupId;
    mtx.unlock();
    return groupId;
}

std::string Batch::getStreamId() {
    mtx.lock();
    std::string streamId = this->streamId;
    mtx.unlock();
    return streamId;
}

int Batch::getLogGroupCount() {
    int count = 0;
    mtx.lock();
    count = this->logGroup->logs.size();
    mtx.unlock();
    return count;
}

void Batch::addLogToLogGroup(Log *log) {
    mtx.lock();
    this->logGroup->logs.push_back(log);
    mtx.unlock();
}

void Batch::addLogsToLogGroup(std::vector<Log *> logs) {
    mtx.lock();
    for (auto log: logs) {
        this->logGroup->logs.push_back(log);
    }
    mtx.unlock();
}


void Batch::addProducerBatchCallBack(CallBack *cb) {

}


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

#include "LogAccumulator.h"
#include "error.h"
#include "common.h"
#include "Batch.h"

const std::string Delimiter = "|";

LogAccumulator::LogAccumulator() {
    this->shutDownFlag.store(false);
}

LogAccumulator::~LogAccumulator() {
}

void LogAccumulator::Init(Config *config, IoWorker *ioWorker, IoThreadPool *threadPool, Producer *producer) {
    this->producerConfig = config;
    this->ioWorker = ioWorker;
    this->threadPool = threadPool;
    this->producer = producer;
}

int LogAccumulator::addLogToProducerBatch(std::string groupId, std::string streamId, std::vector<Log*> log, CallBack *cb) {
    if (this->shutDownFlag.load() == true) {
        return ErrProducerShutDown;
    }

    std::string key = this->getKeyString(groupId, streamId);

    mtx.lock();

    auto result = this->logGroupData.find(key);
    if (result != this->logGroupData.end()) {
        Batch* batch = result->second;

        int64_t logSize = GetLogListSize(log);
        batch->totalDataSize += logSize;
        this->producer->producerLogGroupSize += logSize;

        this->addOrSendProducerBatch(key, groupId, streamId, batch, log, cb);
    } else {
        this->createNewProducerBatch(log, cb, key, groupId, streamId);
    }

    mtx.unlock();

    return 0;
}

std::string LogAccumulator::getKeyString(std::string groupId, std::string streamId) {
    return groupId + Delimiter + streamId + Delimiter;
}

void LogAccumulator::createNewProducerBatch(std::vector<Log*> log, CallBack *cb, std::string key, std::string groupId,
                                            std::string streamId) {
    Batch* batch = new Batch();
    batch->Init(log, cb, groupId, streamId, this->producerConfig);
    this->logGroupData[key] = batch;

    this->producer->producerLogGroupSize += batch->totalDataSize;
}

void LogAccumulator::addOrSendProducerBatch(std::string key, std::string groupId, std::string streamId, Batch *batch,
                                            std::vector<Log*> log, CallBack *cb) {
    int totalDataCount = batch->getLogGroupCount() + 1;

    // 512 KB ~ 5M && 日志数量 <= 4096
    if (batch->totalDataSize > this->producerConfig->MaxBatchSize &&
        batch->totalDataSize < 5 * 1024 * 1024 &&
        totalDataCount <= this->producerConfig->MaxBatchCount ){

        batch->addLogsToLogGroup(log);
        if (cb != nullptr) {
            batch->addProducerBatchCallBack(cb);
        }

        this->innerSendToServer(key, batch);
    } else if (batch->totalDataSize <= this->producerConfig->MaxBatchSize && // < 512KB && 日志数量 <= 4096
                totalDataCount <= this->producerConfig->MaxBatchCount) {
        batch->addLogsToLogGroup(log);
        if (cb != nullptr) {
            batch->addProducerBatchCallBack(cb);
        }
    } else { // 日志大小 > 5M || 日志数量 > 4096
        this->innerSendToServer(key, batch);
        this->createNewProducerBatch(log, cb, key, groupId, streamId);
    }
}

void LogAccumulator::innerSendToServer(std::string key, Batch* batch) {
    this->threadPool->addTask(batch);
    this->logGroupData.erase(key);
}

void LogAccumulator::UnLock() {
    this->mtx.unlock();
}

void LogAccumulator::Lock() {
    this->mtx.lock();
}

std::map<std::string, Batch *> LogAccumulator::GetLogGroupData() {
    return this->logGroupData;
}

Batch *LogAccumulator::find4LogGroupData(std::string key) {
    auto elem = this->logGroupData.find(key);
    if (elem == this->logGroupData.end()) {
        return nullptr;
    }

    return elem->second;
}

void LogAccumulator::delete4LogGroupData(std::string key) {
    this->logGroupData.erase(key);
}

void LogAccumulator::clearLogGroupData() {
    this->logGroupData.clear();
}

void LogAccumulator::Close() {
    this->shutDownFlag.store(true);
}

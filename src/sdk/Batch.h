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

#ifndef CPP_PROJECTS_BATCH_H
#define CPP_PROJECTS_BATCH_H

#include "log.h"
#include "Config.h"
#include "Callback.h"

#include <string>
#include <pthread.h>
#include <vector>
#include <atomic>
#include <mutex>

class Batch {
public:
    Batch();
    ~Batch();

    void Init(std::vector<Log *> log, CallBack *cb, std::string groupId, std::string streamId, Config *config);

    std::atomic<int64_t> totalDataSize;
    std::mutex mtx;
    LogGroup *logGroup;
    int logGroupSize;
    int logGroupCount;
    int attemptCount = 0;

    int64_t baseRetryBackoffMs;
    int64_t nextRetryMs = 0;
    int64_t maxRetryIntervalInMs;
    std::vector<CallBack > callBackList;
    int64_t createTimeMs;
    int maxRetryTimes;
    std::string groupId;
    std::string streamId;
    Result *result = nullptr;
    int maxReservedAttempts;

public:
    bool operator > (const Batch *b) const;

    std::string getGroupId();
    std::string getStreamId();
    int getLogGroupCount();
    void addLogToLogGroup(Log*);
    void addLogsToLogGroup(std::vector<Log*>);
    void addProducerBatchCallBack(CallBack* cb);
};

class batchPointerCompare {
public:
    batchPointerCompare(){}
    ~batchPointerCompare(){}

public:
    bool operator()(const Batch* a, const Batch* b) const {
        return a->operator>(b);
    }
};

#endif //CPP_PROJECTS_BATCH_H

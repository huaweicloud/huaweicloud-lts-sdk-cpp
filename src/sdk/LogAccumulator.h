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

#ifndef LTS_SDK_LOGACCUMULATOR_H
#define LTS_SDK_LOGACCUMULATOR_H


#include <mutex>
#include "IoThreadPool.h"

class LogAccumulator {
public:
    LogAccumulator();
    ~LogAccumulator();
    void Init(Config*, IoWorker*, IoThreadPool*, Producer*);
    int addLogToProducerBatch(std::string groupId, std::string streamId, std::vector<Log*> log, CallBack* cb);

    void Lock();
    void UnLock();

    std::map<std::string, Batch*> GetLogGroupData();
    Batch* find4LogGroupData(std::string key);
    void delete4LogGroupData(std::string key);
    void clearLogGroupData();

    void Close();

private:
    std::string getKeyString(std::string groupId, std::string streamId);
    void createNewProducerBatch(std::vector<Log*> log, CallBack *cb, std::string key, std::string groupId, std::string streamId);
    void addOrSendProducerBatch(std::string key, std::string groupId, std::string streamId, Batch* batch, std::vector<Log*> log, CallBack* cb);
    void innerSendToServer(std::string key, Batch* batch);

private:
    Config* producerConfig;
    IoThreadPool* threadPool;
    IoWorker* ioWorker;
    class Producer* producer;

    std::map<std::string, Batch*> logGroupData;
    std::atomic<bool> shutDownFlag;
    std::mutex mtx;
};


#endif //LTS_SDK_LOGACCUMULATOR_H

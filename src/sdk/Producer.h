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

#ifndef CPP_PROJECTS_PRODUCER_H
#define CPP_PROJECTS_PRODUCER_H


#include "log.h"
#include "Config.h"
#include "Callback.h"

#include <atomic>

class Producer {
public:
    explicit Producer(Config *cfg);

    void Init(void *logAccumulator, void *mover, void *threadPool);

    void Start();

    void Close();

    int SendLog(std::string groupId, std::string streamId, std::vector<Log*> log, CallBack* cb);

private:
    void sendCloseSignal();
    void WaitStart();

public:
    std::atomic<int64_t> producerLogGroupSize;

private:
    void* producerConfig;
    void* logAccumulator;
    void* mover;
    void* threadPool;
    int32_t buckets;
};


extern Producer* InitProducer(Config* cfg);

#endif //CPP_PROJECTS_PRODUCER_H

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

#ifndef CPP_PROJECTS_IOWORKER_H
#define CPP_PROJECTS_IOWORKER_H

#include <map>
#include <semaphore.h>
#include "Result.h"
#include "RetryQueue.h"
#include "Client.h"
#include "Producer.h"

class IoWorker {
public:
    IoWorker();
    ~IoWorker();

    void Init(Client*, RetryQueue*, Config* cfg, Producer*);
    void CloseRetryQueue();
    void sendToServer(Batch*);

    void allocSem();
    void freeSem();

private:
    void processSendSuccess(Batch*, int64_t beginMs);
    void processSendFailed(Batch*, int64_t beginMs, HttpResponse &response);
    void addErrorMessageToBatchAttempt(Batch* batch, HttpResponse& response, bool retryInfo, int64_t beginMs);

private:
    sem_t sem;
    std::atomic<bool> retryQueueShutDownFlag;
    Client *client;
    RetryQueue* retryQueue;
    Producer* producer;

    int64_t taskCount;
    std::map<int, std::string*> noRetryStatusCodeMap;
};


#endif //CPP_PROJECTS_IOWORKER_H

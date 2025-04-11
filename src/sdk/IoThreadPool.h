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

#ifndef LTS_SDK_IOTHREADPOOL_H
#define LTS_SDK_IOTHREADPOOL_H

#include <atomic>
#include "IoWorker.h"
#include "Batch.h"

class IoThreadPool {
public:
    explicit IoThreadPool(IoWorker*);
    ~IoThreadPool();

    void addTask(Batch* batch);
    void start();
    void Close();

    void addThreadCount() {
        threadCount++;
    }
    void delThreadCount() {
        threadCount--;
    }
    bool IsThreadCountZero() {
        return threadCount == 0;
    }
    int getThreadCount() {
        return threadCount;
    }

private:
    std::mutex mtx;
    std::queue<Batch*> queue;
    IoWorker* ioWorker = nullptr;
    std::atomic<bool> threadPoolShutDownFlag;
    std::atomic<int> threadCount;

    Batch *popTask();
    bool hasTask();
};


#endif //LTS_SDK_IOTHREADPOOL_H

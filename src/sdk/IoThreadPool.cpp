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

#include "IoThreadPool.h"
#include "util.h"
#include <thread>

IoThreadPool::IoThreadPool(IoWorker *ioWorker) {
    this->ioWorker = ioWorker;
    this->threadPoolShutDownFlag.store(false);
    this->threadCount = 0;
}

IoThreadPool::~IoThreadPool() {
}

void IoThreadPool::addTask(Batch *batch) {
    mtx.lock();

    this->queue.push(batch);

    mtx.unlock();
}

Batch *IoThreadPool::popTask() {
    Batch* batch = nullptr;

    mtx.lock();

    if (this->queue.size() > 0) {
        batch = this->queue.front();
        this->queue.pop();
    }

    mtx.unlock();

    return batch;
}

bool IoThreadPool::hasTask() {
    bool result = false;

    mtx.lock();

    if (this->queue.size() > 0) {
        result = true;
    }

    mtx.unlock();

    return result;
}

void processTask(IoThreadPool* pool, IoWorker* ioWorker, Batch* batch) {
    ioWorker->sendToServer(batch);
    pool->delThreadCount();
    ioWorker->freeSem();
}

void IoThreadPool::start() {
    addThreadCount();

    while (true) {
        Batch *task = this->popTask();
        if (task != nullptr) {
            this->addThreadCount();

            this->ioWorker->allocSem();
            std::thread t(processTask, this, this->ioWorker, task);

            t.detach();
        } else {
            if (this->threadPoolShutDownFlag.load() == true) {
                break;
            } else {
                SleepMs(100);
            }
        }
    }

    delThreadCount();
}

void IoThreadPool::Close() {
    this->threadPoolShutDownFlag.store(true);
    while (!IsThreadCountZero()) {
        SleepMs(500);
    }
}

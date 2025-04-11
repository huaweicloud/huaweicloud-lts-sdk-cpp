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

#include <thread>
#include "Producer.h"
#include "Client.h"
#include "RetryQueue.h"
#include "IoWorker.h"
#include "IoThreadPool.h"
#include "LogAccumulator.h"
#include "Mover.h"
#include "util.h"
#include "logger.h"

Producer* InitProducer(Config* cfg) {
    Client *client = new Client(cfg);
    RetryQueue *retryQueue = new RetryQueue();
    Producer *producer = new Producer(cfg);

    IoWorker *ioWorker = new IoWorker();
    ioWorker->Init(client, retryQueue, cfg, producer);

    IoThreadPool* pool = new IoThreadPool(ioWorker);

    LogAccumulator* logAccumulator = new LogAccumulator();
    logAccumulator->Init(cfg, ioWorker, pool, producer);

    Mover* mover = new Mover();
    mover->Init(logAccumulator, retryQueue, ioWorker, pool, cfg);

    producer->Init(logAccumulator, mover, pool);

    return producer;
}

Producer::Producer(Config *cfg) {
    this->producerConfig = cfg;
    this->buckets = cfg->Buckets;

    this->producerLogGroupSize = 0;
}

void Producer::Init(void *logAccumulator, void *mover, void *threadPool) {
    this->logAccumulator = (LogAccumulator*)logAccumulator;
    this->mover = (Mover*)mover;
    this->threadPool = (IoThreadPool*)threadPool;
}

void startThreadPool(IoThreadPool* threadPool) {
    threadPool->start();
}

void startMoverThread(Mover* mover) {
    mover->Run();
}

void Producer::WaitStart() {
    IoThreadPool* threadPool = (IoThreadPool*)this->threadPool;
    Mover* mover = (Mover*)this->mover;

    while (threadPool->IsThreadCountZero() || mover->isThreadCountZero()) {
        LogPrint(LOG_LEVEL_WARN, "producer.start() waiting ... pool[%d] mover[%d]", threadPool->getThreadCount(), mover->getThreadCount());
        SleepMs(100);
    }

    LogPrint(LOG_LEVEL_WARN, "producer.start() finished");
}

void Producer::Start() {
    std::thread pool(startThreadPool, (IoThreadPool*)this->threadPool);
    pool.detach();

    std::thread moverThread(startMoverThread, (Mover*)this->mover);
    moverThread.detach();

    this->WaitStart();
}

int Producer::SendLog(std::string groupId, std::string streamId, std::vector<Log*> log, CallBack* cb) {
    LogAccumulator* logAccumulator = (LogAccumulator*)this->logAccumulator;
    return logAccumulator->addLogToProducerBatch(groupId, streamId, log, cb);
}

void Producer::Close() {
    this->sendCloseSignal();
}

void Producer::sendCloseSignal() {
    Mover* m = (Mover*)this->mover;
    LogAccumulator* l = (LogAccumulator*)this->logAccumulator;
    IoWorker* worker = (IoWorker*)this->mover;
    IoThreadPool* pool = (IoThreadPool*)this->threadPool;

    SleepMs(1000);

    worker->CloseRetryQueue(); // RetryQueue 不再接收发送失败日志
    l->Close(); // LogAccumulator 停止消费客户日志
    m->Close(); // 将 LogAccumulator、RetryQueue 中的log全部发送给 IoThreadPool
    pool->Close(); // 将全部 log 通过 https 发送给服务端
}

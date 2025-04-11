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

#include "Config.h"

Config::Config() {
    this->TotalSizeLnBytes = 100 * 1024 * 1024;
    this->MaxIoWorkers = 10;
    this->MaxBlockSec = 60;
    this->MaxBatchSize = 512 * 1024;
    this->LingerMs = 2000;
    this->Retries = 10;
    this->MaxReservedAttempts = 11;
    this->BaseRetryBackoffMs = 100;
    this->MaxRetryBackoffMs = 50 * 1000;
    this->Buckets = 64;
    this->MaxBatchCount = 4096;
    this->NoRetryCodeList = {413, 429};
}

void Config::validateProducerConfig() {
    if (this->MaxReservedAttempts <= 0) {
        this->MaxReservedAttempts = 11;
    }

    if (this->MaxBatchCount > 40960 || this->MaxBatchCount <= 0) {
        this->MaxBatchCount = 40960;
    }

    if (this->MaxBatchSize > 512 * 1024 || this->MaxBatchSize <= 0) {
        this->MaxBatchSize = 512 * 1024;
    }

    if (this->MaxIoWorkers <= 0) {
        this->MaxIoWorkers = 10;
    }

    if (this->BaseRetryBackoffMs <= 0) {
        this->BaseRetryBackoffMs = 100;
    }

    if (this->TotalSizeLnBytes <= 0) {
        this->TotalSizeLnBytes = 100 * 1024 * 1024;
    }

    if (this->LingerMs < 100) {
        this->LingerMs = 2000;
    }
}

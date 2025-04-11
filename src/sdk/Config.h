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

#ifndef CPP_PROJECTS_CONFIG_H
#define CPP_PROJECTS_CONFIG_H

#include <string>
#include <vector>
#include "../client/HttpClient.h"

class Config {
public:
    int64_t TotalSizeLnBytes;
    int64_t MaxIoWorkers;
    int MaxBlockSec;
    int64_t MaxBatchSize;
    int MaxBatchCount;
    int64_t LingerMs;
    int Retries;
    int MaxReservedAttempts;
    int64_t BaseRetryBackoffMs;
    int64_t MaxRetryBackoffMs;
    int32_t Buckets;
    std::string Endpoint;
    std::string AccessKeyID;
    std::string AccessKeySecret;
    std::string ProjectId;
    std::string RegionId;
    std::vector<int> NoRetryCodeList;
    HttpClient *HTTPClient;

public:
    Config();

    void validateProducerConfig();
};


#endif //CPP_PROJECTS_CONFIG_H

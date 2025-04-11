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

#ifndef CPP_PROJECTS_RESULT_H
#define CPP_PROJECTS_RESULT_H

#include <vector>
#include <string>
#include <stdint.h>

class Attempt {
private:
    bool Success;
    int32_t HttpCode;
    std::string RequestId;
    std::string ErrorCode;
    std::string ErrorMessage;
    int64_t TimeStampMs;
    int64_t LastAttemptCostMs;

public:
    Attempt();

    void Init(bool success, std::string requestId, std::string errCode, std::string errorMessage,
              int64_t timeStampMs, int64_t lastAttemptCostMs, int32_t httpCode);
};

class Result {
public:
    Result();

public:
    std::vector<Attempt*> attemptList;
    bool successful;
};


#endif //CPP_PROJECTS_RESULT_H

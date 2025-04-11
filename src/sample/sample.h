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

#ifndef LTS_SDK_SAMPLE_H
#define LTS_SDK_SAMPLE_H

#include <string>

const int PARAM_NUM = 14;

class Argument {
public:
    std::string param[PARAM_NUM];

    std::string endpoint;
    std::string ak;
    std::string sk;
    std::string region;
    std::string projectId;
    std::string log;
    std::string key;
    std::string value;
    std::string groupId;
    std::string streamId;
    std::string queueSize;
    std::string timeLimit;
    std::string timeOut;

    int count;
    int queueSizeArg;
    int timeLimitArg;
    int timeOutArg;

public:
    bool parseParameters(char c, std::string value);
    bool check();
};

void TestSendLog();

void TestSDK(Argument);

void TestJson();

void TestThread();

void TestCallback();

void TestDeleteObj();

void TestObjSize();

#endif //LTS_SDK_SAMPLE_H

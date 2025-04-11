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

#include "util.h"
#include "JsonValue.h"
#include <time.h>
#include <unistd.h>

int64_t UnixNano() {
    struct timespec now;

    if (0 != clock_gettime(CLOCK_REALTIME, &now)) {
        return 0;
    }

    int64_t nao = now.tv_sec * 1000000000 + now.tv_nsec;

    return nao;
}

int64_t GetTimeMs(int64_t unixNano) {
    return unixNano / 1000 / 1000;
}

std::string mapToJson(std::map<std::string, std::string> input) {
    JsonValue value;
    Json::Value &jv = value.getValue();
    for (auto &d: input) {
        jv[d.first] = d.second;
    }
    return value.toString();
}

void SleepMs(int64_t ms) {
    usleep(ms * 1000);
}

void SleepUs(int64_t us) {
    usleep(us);
}

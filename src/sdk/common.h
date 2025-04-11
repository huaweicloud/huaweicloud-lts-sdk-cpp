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

#ifndef CPP_PROJECTS_COMMON_H
#define CPP_PROJECTS_COMMON_H

#include <chrono>
#include "log.h"

static inline int GetLogSizeCalculate(Log *log) {
    int sizeInBytes = 4;
    std::vector<LogContent*> logContent = log->GetContents();
    size_t count = logContent.size();

    for (int i = 0; i < count; ++i) {
        sizeInBytes += logContent[i]->Log.size();
    }

    return sizeInBytes;
}

static inline int GetLogListSize(std::vector<Log*> logList) {
    int sizeInBytes = 0;

    for (auto &d: logList) {
        sizeInBytes += GetLogSizeCalculate(d);
    }

    return sizeInBytes;
}

#endif //CPP_PROJECTS_COMMON_H

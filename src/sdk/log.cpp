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
#include "log.h"
#include "util.h"

Log* GenerateLog(std::vector<std::string> addLogMap, std::map<std::string, std::string> labels) {
    Log *log = new Log;

    for (auto &content: addLogMap) {
        LogContent* logContent = new LogContent();
        logContent->Log = content;
        logContent->LogTimeNs = UnixNano();
        log->Contents.push_back(logContent);
    };

    std::string labelStr = "{}";
    if (labels.size() != 0) {
        labelStr = mapToJson(labels);
    }

    log->Labels = labelStr;

    return log;
}

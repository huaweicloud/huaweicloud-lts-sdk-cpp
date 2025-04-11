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

#ifndef CPP_PROJECTS_LOG_H
#define CPP_PROJECTS_LOG_H

#include <vector>
#include <string>
#include <map>
#include "JsonValue.h"

static inline int calcSize(uint64_t x) {
    int result = 0;
    for (;;) {
        result++;
        x >>= 7;
        if (x == 0) {
            break;
        }
    }
    return result;
}

class LogTag {
public:
    std::string Key;
    std::string Value;

    int Size() {
        int result = 0;

        if (this->Key != "") {
            int l = this->Key.size();
            result += 1 + l + calcSize(l);
        }

        if (this->Value != "") {
            int l = this->Value.size();
            result += 1 + l + calcSize(l);
        }

        return result;
    }
    std::string ToJson() {
        JsonValue value;
        Json::Value &jv = value.getValue();
        if (Key != "") {
            jv["Key"] = Key;
        }
        if (Value != "") {
            jv["Value"] = Value;
        }
        return value.toString();
    }
};

class LogContent {
public:
    int64_t LogTimeNs;
    std::string Log;

    int Size() {
        return this->Log.size() + calcSize(uint64_t(this->LogTimeNs));
    }

    std::string ToJson() {
        JsonValue value;
        Json::Value &jv = value.getValue();
        if (LogTimeNs != 0) {
            jv["log_time_ns"] = (double)LogTimeNs;
        }
        if (Log != "") {
            jv["log"] = Log;
        }
        return value.toString();
    }
};

class LogItem {
};

class LogItems {
};

class Log {
public:
    uint32_t Time;
    std::vector<LogContent*> Contents;
    std::string Labels;
    std::string ProjectId;

    ~Log() {
        while (!Contents.empty()) {
            LogContent* content = Contents.back();
            Contents.pop_back();
            delete content;
        }
    }

    std::vector<LogContent*> GetContents() {
        return this->Contents;
    }

    int Size() {
        int result = 0;

        result += 1 + calcSize(uint64_t(sizeof this->Time));

        for (auto e: this->Contents) {
            int l = e->Size();
            result += 1 + l + calcSize(uint64_t(l));
        }
        return result;
    }

    std::string ToJson() {
        JsonValue value;
        Json::Value &jv = value.getValue();
        for (int i = 0; i < Contents.size(); ++i) {
            LogContent* logContent = Contents[i];
            jv["contents"][i] = logContent->ToJson();
        }
        if (Time != 0) {
            jv["time"] = Time;
        }
        if (ProjectId != "") {
            jv["tenant_project_id"] = ProjectId;
        }
        if (Labels != "") {
            jv["labels"] = Labels;
        }
        return value.toString();
    }
};

class LogGroup {
public:
    std::vector<Log*> logs;
    std::string CateGory;
    std::vector<LogTag*> logTags;

    LogGroup() {
    }

    ~LogGroup() {
        while (!this->logs.empty()) {
            Log* log = this->logs.back();
            this->logs.pop_back();
            delete log;
        }
    }

    void Init(std::vector<Log*> logs) {
        for (auto d: logs) {
            this->logs.push_back(d);
        }
    }

    int Size() {
        int result = 0;

        for (auto log: this->logs) {
            int l = log->Size();
            result += 1 + l + calcSize(uint64_t(l));
        }

        {
            int l = this->CateGory.size();
            result += 1 + this->CateGory.size() + calcSize(l);
        }

        for (auto tag: this->logTags) {
            int l = tag->Size();
            result += 1 + l + calcSize(l);
        }

        return result;
    }

    std::vector<Log*> GetLogs() {
        return this->logs;
    }

    std::string ToJson() {
        JsonValue value;
        Json::Value &jv = value.getValue();
        for (int i = 0; i < logs.size(); ++i) {
            Log* log = logs[i];
            {
                if (log->Time != 0) {
                    jv["log_items"][i]["time"] = log->Time;
                }
                if (log->ProjectId != "") {
                    jv["log_items"][i]["tenant_project_id"] = log->ProjectId;
                }
                if (log->Labels != "") {
                    jv["log_items"][i]["labels"] = log->Labels;
                }
                for (int j = 0; j < log->Contents.size(); ++j) {
                    LogContent* logContent = log->Contents[j];
                    {
                        if (logContent->LogTimeNs != 0) {
                            jv["log_items"][i]["contents"][j]["log_time_ns"] = (double)logContent->LogTimeNs;
                        }
                        if (logContent->Log != "") {
                            jv["log_items"][i]["contents"][j]["log"] = logContent->Log;
                        }
                    }
                }
            }
        }
        if (CateGory != "") {
            jv["Category"] = CateGory;
        }
        for (int i = 0; i < logTags.size(); ++i) {
            LogTag* logTag = logTags[i];
            {
                if (logTag->Key != "") {
                    jv["LogTags"][i]["Key"] = logTag->Key;
                }
                if (logTag->Value != "") {
                    jv["LogTags"][i]["Value"] = logTag->Value;
                }
            }
        }
        return value.toString();
    }
};

Log* GenerateLog(std::vector<std::string> addLogMap, std::map<std::string, std::string> labels);

#endif //CPP_PROJECTS_LOG_H

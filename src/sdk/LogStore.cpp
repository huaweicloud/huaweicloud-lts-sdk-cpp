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

#include <iostream>
#include "LogStore.h"
#include "../client/logger.h"
#include "../client/Request.h"

using namespace std;

LogStore::LogStore() {

}

LogStore::~LogStore() {
    if (this->logProject != nullptr) {
        delete this->logProject;
    }
}

void LogStore::Init(LogProject *logProject, enum CompressType compressType, std::string groupId, std::string streamId) {
    this->logProject = logProject;
    this->putLogCompressType = compressType;
    this->GroupId = groupId;
    this->StreamId = streamId;
}

int LogStore::PutLogs(LogGroup *logGroup, HttpResponse& response) {
    std::string body = logGroup->ToJson();
    LogPrint(LOG_LEVEL_DEBUG, "body [%s]", body.c_str());

    string urlPath = "/v2/internal/" + this->logProject->ProjectId +
            "/lts/groups/" + this->GroupId +
            "/streams/"+  this->StreamId + "/tenant/batch-contents";
    string endPoint = this->logProject->Endpoint;
    if (endPoint.compare(0, 4, "http") != 0) {
        endPoint = "https://" + endPoint;
    }

    map<string, string> headers;
    headers["content-type"] = "application/json";
    headers["Accept"] = "application/json;charset=utf-8";

    Param p{this->logProject->AccessKeyID,
            this->logProject->AccessKeySecret,
            endPoint,
            urlPath,
            this->logProject->Region,
            "POST",
            headers,
            body};

    Request req;
    int ret = req.request(p, response);
    if (ret != 0) {
        LogPrint(LOG_LEVEL_ERROR, "request failed[%d]", ret);
    } else {
        LogPrint(LOG_LEVEL_WARN, "http send %d log success", logGroup->logs.size());
    }

    return ret;
}

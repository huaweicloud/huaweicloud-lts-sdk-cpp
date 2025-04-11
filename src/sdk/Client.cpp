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

#include "LogProject.h"
#include "Client.h"
#include "LogStore.h"

Client::Client(Config *cfg) {
    this->Endpoint = cfg->Endpoint;
    this->AccessKeyID = cfg->AccessKeyID;
    this->AccessKeySecret = cfg->AccessKeySecret;
    this->Region = cfg->RegionId;
    this->ProjectId = cfg->ProjectId;
}

int Client::PutLogs(std::string groupId, std::string streamId, LogGroup *logGroup, HttpResponse &response) {
    LogProject* logProject = new LogProject();
    logProject->Init(this);

    LogStore* logStore = new LogStore();
    logStore->Init(logProject, CompressNone, groupId, streamId);

    int ret = logStore->PutLogs(logGroup, response);

    delete logStore;

    return ret;
}


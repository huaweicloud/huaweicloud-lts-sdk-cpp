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

#include "Request.h"
#include "HttpClient.h"
#include "logger.h"
#include "Common.h"
#include "Signer.h"
#include "../sdk/JsonValue.h"

using namespace std;

void Request::generateHttpRequest(HttpRequest &request, Param &p) {
    request.url = p.endPoint + p.urlPath;
    request.urlPath = p.urlPath;
    request.body = p.body;
    request.method = p.method;

    for (auto &d: p.headers) {
        request.headers[d.first] = d.second;
    }

    string host = getHostFromUrl(request.url);
    LogPrint(LOG_LEVEL_DEBUG, "host: %s", host.c_str());
    request.headers["Host"] = host;

    string ak = p.ak;
    string sk = p.sk;
    string derivedAuthServiceName = "lts";
    string regionId = p.regionId;

    SignHeaderBasic(request, ak, sk, derivedAuthServiceName, regionId);

    auto it = request.headers.find("Authorization");
    if (it != request.headers.end()) {
        LogPrint(LOG_LEVEL_DEBUG, "Authorization [%s]", it->second.c_str());
    } else {
        LogPrint(LOG_LEVEL_ERROR, "Authorization not found");
    }
    auto it2 = request.headers.find("X-Sdk-Date");
    if (it2 != request.headers.end()) {
        LogPrint(LOG_LEVEL_DEBUG, "X-Sdk-Date [%s]", it2->second.c_str());
    } else {
        LogPrint(LOG_LEVEL_ERROR, "X-Sdk-Date not found");
    }
}

int Request::parseHttpBody(HttpResponse &response) {
    JsonValue jv;

    bool ret = jv.parse(response.body);
    if (!ret) {
        return false;
    }

    string errorCode = jv.getValue().get("errorCode", "").asString();
    string errorMessage = jv.getValue().get("errorMessage", "").asString();
    string requestID = jv.getValue().get("requestID", "").asString();

    response.errorCode = errorCode;
    response.errorMessage = errorMessage;
    response.requestId = requestID;

    return true;
}

int Request::request(Param &p, HttpResponse &response) {
    HttpRequest request;

    generateHttpRequest(request, p);

    HttpClient* client = new HttpClient();

    int ret = client->post(request, response);

    parseHttpBody(response);

    LogPrint(LOG_LEVEL_DEBUG, "ret[%d] httpCode[%d] header[%s] body[%s] ", ret, response.HttpCode, response.head.c_str(), response.body.c_str());
    LogPrint(LOG_LEVEL_DEBUG, "parseHttpBody [%s] [%s]", "errorCode", response.errorCode.c_str());
    LogPrint(LOG_LEVEL_DEBUG, "parseHttpBody [%s] [%s]", "errorMessage", response.errorMessage.c_str());
    LogPrint(LOG_LEVEL_DEBUG, "parseHttpBody [%s] [%s]", "requestID", response.requestId.c_str());

    delete client;

    return ret;
}

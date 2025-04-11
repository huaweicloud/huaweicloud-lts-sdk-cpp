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

#ifndef LTS_SDK_REQUEST_H
#define LTS_SDK_REQUEST_H

#include <string>
#include <map>
#include "HttpClient.h"


struct Param {
    std::string ak;
    std::string sk;
    std::string endPoint;
    std::string urlPath;
    std::string regionId;
    std::string method;
    std::map<std::string, std::string> headers;
    std::string body;
};

class Request {
public:
    int request(Param &p, HttpResponse &response);

private:
    int parseHttpBody(HttpResponse &response);
    void generateHttpRequest(HttpRequest &request, Param &p);
};

extern void testRequest();

#endif //LTS_SDK_REQUEST_H

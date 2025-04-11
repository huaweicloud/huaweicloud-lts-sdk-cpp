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

#ifndef LTS_SDK_HTTPCLIENT_H
#define LTS_SDK_HTTPCLIENT_H

#include <string>
#include <map>
#include <sstream>
#include "utils.h"

namespace defaults{
    static const std::string sdk_signing_algorithm = "SDK-HMAC-SHA256";
    static const std::string dateFormat = "X-Sdk-Date";
    static const std::string host = "Host";
    static const std::string Authorization = "Authorization";
}

struct HttpRequest {
    std::string url;
    std::string urlPath;
    std::string body;
    std::string method;
    std::map <std::string, std::string> headers;

    std::string InitHeaders(time_t now) {
        std::string datetime = toISO8601Time(now);
        headers[defaults::dateFormat] = datetime;
        return datetime;
    }
};

struct HttpResponse {
    std::string head = "";
    std::string body = "";
    int64_t HttpCode = 0;
    std::string errorCode = "";
    std::string errorMessage = "";
    std::string requestId = "";
};

struct SockAddress {
    std::string host;
    int port;

    SockAddress() {
        port = 0;
    }

    SockAddress(const std::string &hst, int pt) {
        host = hst;
        port = pt;
    }

    bool operator<(const SockAddress &address) const {
        if (host != address.host) {
            return host < address.host;
        }
        return port < address.port;
    }

    bool operator==(const SockAddress &address) const {
        if (host == address.host && port == address.port) {
            return true;
        }
        return false;
    }

    void reset() {
        host.clear();
        port = 0;
    }

    std::string toString() const {
        std::ostringstream sout;
        sout << host << ":" << port;
        return sout.str();
    }
};

class HttpClient {
public:
    HttpClient();

    ~HttpClient();

public:

    int get(const HttpRequest &request, HttpResponse &response);

    int get(const HttpRequest &request, HttpResponse &response, const SockAddress &proxy);

    int post(HttpRequest &request, HttpResponse &response);

    int post(HttpRequest &request, HttpResponse &response, const SockAddress &proxy);

private:
    int genSecHeaders(std::map <std::string, std::string> &secHeaders);

};

#endif //LTS_SDK_HTTPCLIENT_H

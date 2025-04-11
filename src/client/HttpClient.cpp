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

#include "HttpClient.h"
#include "logger.h"
#include "../../opensource_build/curl-install/include/curl/curl.h"
#include "Common.h"
//#include <curl/curl.h>

using namespace std;

HttpClient::HttpClient() {
}

HttpClient::~HttpClient() {

}

static size_t writeFunc(const void *buffer, size_t size, size_t cnt, void *userdata) {

    if (buffer == NULL || userdata == NULL) {
        return 0;
    }

    HttpResponse *res = static_cast<HttpResponse *>(userdata);
    res->body.append((char *) buffer, size * cnt);
    return cnt;
}

static size_t headerFunc(const void *buffer, size_t size, size_t cnt, void *userdata) {

    if (buffer == NULL || userdata == NULL) {
        return 0;
    }

    HttpResponse *res = static_cast<HttpResponse *>(userdata);
    res->head.append((const char *) buffer, size * cnt);
    return cnt;
}

int HttpClient::get(const HttpRequest &request, HttpResponse &response) {
    SockAddress proxy;
    return get(request, response, proxy);
}

int HttpClient::get(const HttpRequest &request, HttpResponse &response, const SockAddress &proxy) {
    CURLcode res;
    CURL *curl = curl_easy_init();

    if (NULL == curl) {
        return CURLE_FAILED_INIT;
    }

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json;charset=utf-8");

    string host = getHostFromUrl(request.url);
    headers = curl_slist_append(headers, ("Host: " + host).c_str());

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, request.url.c_str());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &response);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

    if (!proxy.host.empty() && proxy.port > 0) {
        curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5_HOSTNAME);
        curl_easy_setopt(curl, CURLOPT_PROXY, proxy.host.c_str());
        curl_easy_setopt(curl, CURLOPT_PROXYPORT, proxy.port);
        LogPrint(LOG_LEVEL_INFO, "get with proxy %s:%d ", proxy.host.c_str(), proxy.port);
    }

    res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        LogPrint(LOG_LEVEL_WARN, "get from %s failed...%s", host.c_str(), curl_easy_strerror(res));
        return -1;
    }

    LogPrint(LOG_LEVEL_DEBUG, "get from %s ... ", host.c_str());
    return 0;
}

int HttpClient::post(HttpRequest &request, HttpResponse &response) {
    SockAddress proxy;
    return post(request, response, proxy);
}

int HttpClient::post(HttpRequest &request, HttpResponse &response, const SockAddress &proxy) {

    LogPrint(LOG_LEVEL_INFO, "post to %s ... ", request.url.c_str());

    CURLcode res;
    CURL *curl = curl_easy_init();

    if (NULL == curl) {
        LogPrint(LOG_LEVEL_ERROR, "init curl failed ...");
        return -1;
    }

    struct curl_slist *headers = NULL;;
    for (map<string, string>::iterator it = request.headers.begin(); it != request.headers.end(); ++it) {
        string str = it->first + string(": ") + it->second;
        headers = curl_slist_append(headers, str.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, request.url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerFunc);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *) &response);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &response);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);


    if (!proxy.host.empty() && proxy.port > 0) {
        curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5_HOSTNAME);
        curl_easy_setopt(curl, CURLOPT_PROXY, proxy.host.c_str());
        curl_easy_setopt(curl, CURLOPT_PROXYPORT, proxy.port);
        LogPrint(LOG_LEVEL_INFO, "post with proxy %s:%d ", proxy.host.c_str(), proxy.port);
    }

    res = curl_easy_perform(curl);

    // 获取HTTP状态码
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.HttpCode);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (response.HttpCode != 200) {
        LogPrint(LOG_LEVEL_WARN, "post to url[%s] failed \n  err[%s] \n  code[%d]\n  response[%s]\n",
                 request.url.c_str(), curl_easy_strerror(res), response.HttpCode, response.body.c_str());
        return -1;
    }

    return 0;
}

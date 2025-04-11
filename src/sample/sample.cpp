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

#include "sample.h"
#include <iostream>
#include <thread>
#include "../sdk/Config.h"
#include "../sdk/Producer.h"
#include "../sdk/util.h"
#include "../client/Common.h"
#include "../client/logger.h"
#include "../client/Signer.h"
#include "JsonValue.h"
#include "common.h"

using namespace std;

Config* GetConfig(Argument p) {
    Config* cfg = new Config();

    cfg->Endpoint = p.endpoint;
    cfg->AccessKeyID = p.ak;
    cfg->AccessKeySecret = p.sk;
    cfg->RegionId = p.region;
    cfg->ProjectId = p.projectId;
    cfg->MaxIoWorkers = p.queueSizeArg;
    cfg->LingerMs = p.timeLimitArg;

    cfg->validateProducerConfig();

    return cfg;
}

Log* generateLog(Argument &p) {
    std::vector<std::string> logContentArr = {p.log};
    std::map<std::string, std::string> labels = {{p.key, p.value}};

    return GenerateLog(logContentArr, labels);
}

void SuccessCallBack(Result* result) {
    if (result == nullptr) {
        return;
    }

    const char* isSuccess = "false";
    if (result->successful) {
        isSuccess = "true";
    }

    LogPrint(LOG_LEVEL_DEBUG, "SuccessCallBack [%s]", isSuccess);
}

void FailCallBack(Result* result) {
    if (result == nullptr) {
        return;
    }

    const char* isSuccess = "false";
    if (result->successful) {
        isSuccess = "true";
    }

    LogPrint(LOG_LEVEL_DEBUG, "SuccessCallBack [%s]", isSuccess);
}

void TestSDK(Argument p) {
    Config *cfg = GetConfig(p);

    Producer *producerInstance = InitProducer(cfg);
    producerInstance->Start();

    CallBack *cb = new CallBack();
    cb->Success = SuccessCallBack;
    cb->Fail = FailCallBack;

    for (int i = 0; i < p.count; ++i) {
        Log *log = generateLog(p);
        producerInstance->SendLog(p.groupId, p.streamId, std::vector<Log *>{log}, cb);
    }

    SleepMs(p.timeOutArg * 60 * 1000);

    producerInstance->Close();

    delete cb;

    delete cfg;

    cout << "sample() end" << endl;
}

void generateHttpRequest(HttpRequest &request) {
    string ProjectId = "projectId";

    // gama
    string GroupId = "groupId";
    string StreamId = "streamId";

    string urlPath = "/v2/internal/" + ProjectId + "/lts/groups/" + GroupId + "/streams/"+ StreamId +"/tenant/batch-contents";

    string url = "https://xx" + urlPath;

    string body = "{\"log_items\":[{\"contents\":[{\"log_time_ns\":1719121280262421400,\"log\":\"this is no.[0] log\"}],\"labels\":\"{\\\"keyA\\\":\\\"valueA\\\",\\\"keyB\\\":\\\"valueB\\\",\\\"keyC\\\":\\\"valueC\\\"}\"}]}";

    request.url = url;
    request.urlPath = urlPath;
    request.body = body;
    request.method = "POST";

    request.headers["content-type"] = "application/json";
    request.headers["Accept"] = "application/json;charset=utf-8";
    string host = getHostFromUrl(request.url);
    LogPrint(LOG_LEVEL_DEBUG, "host: %s", host.c_str());
    request.headers["Host"] = host.c_str();

    string ak = "ak";
    string sk = "sk";
    string derivedAuthServiceName = "lts";
    string regionId = "regionId";

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

void TestSendLog() {
    HttpRequest request;
    HttpResponse response;

    generateHttpRequest(request);

    HttpClient* client = new HttpClient();
    client->post(request, response);

    LogPrint(LOG_LEVEL_WARN, "header[%s] body[%s]", response.head.c_str(), response.body.c_str());

    delete client;
}

class Person {
public:
    string name;
    int age;
    vector<Person> friends;

    string toJson() {
        JsonValue value;
        Json::Value &jv = value.getValue();
        jv["Name"] = name;
        jv["Age"] = age;
        for (int i = 0; i < friends.size(); ++i) {
            Person f = friends[i];
            jv["Friends"][i] = f.toJson();
        }
        return value.toString();
    }
};

class Family {
public:
    Person jack;
    vector<Person> child;

    string toJson() {
        JsonValue value;

        Json::Value &jv = value.getValue();
        for (int i = 0; i < jack.friends.size(); ++i) {
            Person f = jack.friends[i];
            jv["Jack"]["Friends"][i]["Name"] = f.name;
            jv["Jack"]["Friends"][i]["Age"] = f.age;
        }

        for (int i = 0; i < child.size(); ++i) {
            auto p = child[i];
            jv["Child"][i]["Name"] = p.name;
            jv["Child"][i]["Age"] = p.age;
            for (int j = 0; j < p.friends.size(); ++j) {
                Person f = p.friends[j];
                jv["Child"][i]["Friends"][j]["Name"] = f.name;
                jv["Child"][i]["Friends"][j]["Age"] = f.age;
            }
        }

        return value.toString();
    }
};

void TestJsonUnmarshal() {
    string str = "{\"errorCode\":\"SVCSTG.ALS.200.200\",\"errorMessage\":\"Report success.\",\"result\":null, \"requestID\":\"1\"}";
    str = "{}";
    JsonValue jv;

    bool ret = jv.parse(str);
    if (!ret) {
        return;
    }

    string errorCode = jv.getValue().get("errorCode", "").asString();
    string errorMessage = jv.getValue().get("errorMessage", "").asString();
    string requestID = jv.getValue().get("requestID", "").asString();

    LogPrint(LOG_LEVEL_WARN, "[%s] [%s]", "errorCode", errorCode.c_str());
    LogPrint(LOG_LEVEL_WARN, "[%s] [%s]", "errorMessage", errorMessage.c_str());
    LogPrint(LOG_LEVEL_WARN, "[%s] [%s]", "requestID", requestID.c_str());
}

void TestJsonMarshal() {
    Person p;
    p.name = "bigman";
    p.age = 18;

    Family f;
    f.jack = p;

    Person c{"a", 1};
    Person f1{"f1", 1};
    Person f2{"f2", 2};
    c.friends.push_back(f1);
    c.friends.push_back(f2);

    Person c2{"b", 2};
    f.child.push_back(c);
    f.child.push_back(c2);

    LogPrint(LOG_LEVEL_WARN, "json [%s]", p.toJson().c_str());
    LogPrint(LOG_LEVEL_WARN, "json [%s]", f.toJson().c_str());
}

void TestJson() {
    TestJsonUnmarshal();
}

class threadCtrl;
void startMoverThread2(threadCtrl* ctl);

class threadCtrl {
public:
    std::atomic<int> threadCount;
    std::atomic<bool> moverShutDownFlag;

public:
    threadCtrl() {
        threadCount = 0;
        moverShutDownFlag.store(false);
    }

    ~threadCtrl() {
    }

public:
    void addThreadCount() {
        threadCount++;
    }
    void subThreadCount() {
        threadCount--;
    }
    bool isThreadCountZero() {
        return threadCount == 0;
    }
    int getThreadCount() {
        return threadCount;
    }

    void Close() {
        this->moverShutDownFlag.store(true);
        while (!isThreadCountZero()) {
            int num = getThreadCount();
            LogPrint(LOG_LEVEL_WARN, "close() waiting ... thread number [%d] process ...", num);
            SleepMs(100);
        }
        LogPrint(LOG_LEVEL_WARN, "close() exit ...");
    }

    void Run() {
        addThreadCount();

        for (;this->moverShutDownFlag.load() != true;) {
            if (this->moverShutDownFlag.load()) {
                LogPrint(LOG_LEVEL_WARN, "run() exit ...");
                SleepMs(100);
                break;
            }

            std::thread moverThread(startMoverThread2, this);
            moverThread.detach();

            LogPrint(LOG_LEVEL_WARN, "run() process ...");
            SleepMs(100);
        }

        LogPrint(LOG_LEVEL_WARN, "run() exit ...");
        subThreadCount();
    }

};

void startMoverThread2(threadCtrl* ctl) {
    int id = ctl->threadCount.load();
    LogPrint(LOG_LEVEL_WARN, "thread [%d] start ...", id);
    ctl->addThreadCount();
    SleepMs(1000);
    ctl->subThreadCount();
    LogPrint(LOG_LEVEL_WARN, "thread [%d] exit ...", id);
}

void startMoverThread(threadCtrl* mover) {
    LogPrint(LOG_LEVEL_WARN, "main start ...");
    mover->Run();
    LogPrint(LOG_LEVEL_WARN, "main exit ...");
}

void Close2(threadCtrl *ctrl) {
    ctrl->moverShutDownFlag.store(true);
    while (!ctrl->isThreadCountZero()) {
        int num = ctrl->getThreadCount();
        LogPrint(LOG_LEVEL_WARN, "close() waiting ... thread number [%d] process ...", num);
        SleepMs(100);
    }
    LogPrint(LOG_LEVEL_WARN, "close() exit ...");
}

void TestThread() {
    threadCtrl *ctrl = new threadCtrl;

    std::thread moverThread(startMoverThread, ctrl);
    moverThread.detach();

    SleepMs(1000);
    Close2(ctrl);

    delete ctrl;
}

typedef void (*callbackx)(void*);

void callback1(void*) {
    LogPrint(LOG_LEVEL_WARN, "callback1");
}

void callback2(void*) {
    LogPrint(LOG_LEVEL_WARN, "callback2");
}

class CallBack2 {
public:
    callbackx cb;
    callbackx cb2;
};

void TestCallback() {
    CallBack2 cb{callback1, callback2};
    CallBack2 cb2 = cb;
    cb2.cb(nullptr);
    cb2.cb2(nullptr);
}

class C {
public:
    string name;
};

class B {
public:
    ~B() {
        LogPrint(LOG_LEVEL_WARN, "delete c");
        delete c;
    }
    C *c;
};

class A {
public:
    ~A() {
        for (int i = 0; i < b.size(); ++i) {
            LogPrint(LOG_LEVEL_WARN, "delete b[%d]", i);
            delete b[i];
        }
    }
public:
    vector<B*> b;
};

void TestDeleteObj() {
    C* c = new C{"hello"};

    B* b = new B;
    b->c = c;

    A *a = new A;
    a->b.push_back(b);

    delete a;
}

void TestObjSize() {
    // LogTag
    LogTag *tag = new LogTag{"01234567", "76543210"};
    LogPrint(LOG_LEVEL_WARN, "logTag size[%d]", tag->Size());

    LogTag tag2{"", "76543210"};
    LogPrint(LOG_LEVEL_WARN, "logTag2 size[%d]", tag2.Size());

    // LogContent
    LogContent *logContent = new LogContent{123, "0123456701234567"};
    LogPrint(LOG_LEVEL_WARN, "logContent size[%d]", logContent->Size());

    LogContent logContent2{123, ""};
    LogPrint(LOG_LEVEL_WARN, "logContent2 size[%d]", logContent2.Size());

    // Log
    Log *log = new Log{123, vector<LogContent*>{logContent}, "labels", "projectId"};
    LogPrint(LOG_LEVEL_WARN, "log size[%d]", log->Size());

    LogGroup* logGroup = new LogGroup();
    logGroup->logs.push_back(log);
    logGroup->CateGory = "1234567";
    logGroup->logTags.push_back(tag);
    LogPrint(LOG_LEVEL_WARN, "logGroup size[%d]", logGroup->Size());

    int64_t logSize = GetLogListSize(vector<Log*>{log, log});
    LogPrint(LOG_LEVEL_WARN, "logSize size[%d]", logSize);
}

bool Argument::parseParameters(char c, std::string value) {
    string opArr = "aserplkvgmnqth";
    for (int i = 0; i < opArr.size(); ++i) {
        if (opArr[i] == c) {
            param[i] = value;
            return true;
        }
    }
    return false;
}

bool Argument::check() {
    for (int i = 0; i < PARAM_NUM; ++i) {
        if (param[i] == "") {
            return false;
        }
    }

    ak = param[0];
    sk = param[1];
    endpoint = param[2];
    region = param[3];
    projectId = param[4];
    log = param[5];
    key = param[6];
    value = param[7];
    groupId = param[8];
    streamId = param[9];

    count = atoi(param[10].c_str());
    if (count < 1) {
        return false;
    }

    queueSizeArg = atoi(param[11].c_str());
    if (queueSizeArg < 1) {
        return false;
    }

    timeLimitArg = atoi(param[12].c_str());
    if (timeLimitArg < 1) {
        return false;
    }

    timeOutArg = atoi(param[13].c_str());
    if (timeOutArg < 1) {
        return false;
    }

    LogPrint(LOG_LEVEL_WARN, "ak %s", ak.c_str());
    LogPrint(LOG_LEVEL_WARN, "sk %s", sk.c_str());
    LogPrint(LOG_LEVEL_WARN, "endpoint %s", endpoint.c_str());
    LogPrint(LOG_LEVEL_WARN, "region %s", region.c_str());
    LogPrint(LOG_LEVEL_WARN, "projectId %s", projectId.c_str());
    LogPrint(LOG_LEVEL_WARN, "log %s", log.c_str());
    LogPrint(LOG_LEVEL_WARN, "key %s", key.c_str());
    LogPrint(LOG_LEVEL_WARN, "value %s", value.c_str());
    LogPrint(LOG_LEVEL_WARN, "groupId %s", groupId.c_str());
    LogPrint(LOG_LEVEL_WARN, "streamId %s", streamId.c_str());
    LogPrint(LOG_LEVEL_WARN, "count %d", count);
    LogPrint(LOG_LEVEL_WARN, "queueSizeArg %d", queueSizeArg);
    LogPrint(LOG_LEVEL_WARN, "timeLimitArg %d ms", timeLimitArg);
    LogPrint(LOG_LEVEL_WARN, "timeOutArg %d minutes", timeOutArg);

    return true;
}

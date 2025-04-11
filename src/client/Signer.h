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

#ifndef LTS_SDK_SIGNER_H
#define LTS_SDK_SIGNER_H

#include <string>
#include <vector>
#include "HttpClient.h"
#include "hasher.h"

int SignHeaderBasic(HttpRequest &request, std::string ak, std::string sk, std::string derivedAuthServiceName, std::string regionId);


#define DerivationAlgorithm "V11-HMAC-SHA256"

class Signer {
private:
    /* Credentials */
    std::string mAppKey;
    std::string mAppSecret;
    std::string derivedAuthServiceName;
    std::string regionId;
    Hasher* hasher;

public:
    Signer();
    Signer(std::string appKey, std::string appSecret, std::string derivedAuthServiceName, std::string regionId);
    ~Signer();

    std::vector<std::string> getSignedHeaders(std::map<std::string, std::string> &headers);
    std::string getCanonicalQueryString(HttpRequest &request);
    std::string getCanonicalURI(std::string& uri);
    std::string getHexHash(std::string& payload);
    std::string fmtSignedHeaders(std::vector<std::string> &signedHeaders);
    std::string generateInfoString(time_t t);
    std::string DerivationAuthHeaderValue(std::string signature, std::string accessKey,
                                                  std::string info, std::vector<std::string> signedHeaders);
    std::string SignStringToSign(std::string stringToSign, std::string signingKey);
    std::string GetDerivationKey(std::string accessKey, std::string secretKey, std::string info);
    std::string StringToSignDerived(std::string canonicalRequest, std::string info, time_t t);
    std::string getCanonicalHeaders(HttpRequest &request, std::vector<std::string> signedHeader);
    std::string getCanonicalRequest(HttpRequest &request, std::vector<std::string> signedHeader);

    // One stroke create signature
    void createSignature(HttpRequest &request);
};

#endif //LTS_SDK_SIGNER_H

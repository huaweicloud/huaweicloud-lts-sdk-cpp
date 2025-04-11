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

#include "Signer.h"
#include "openssl/sha.h"
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/kdf.h>
#include <string.h>
#include <vector>
#include "algorithm"
#include "logger.h"
#include <iostream>

using namespace std;

int SignHeaderBasic(HttpRequest &request, string ak, string sk, string derivedAuthServiceName, string regionId) {
    Signer sign(ak, sk, derivedAuthServiceName, regionId);
    sign.createSignature(request);
}

Signer::Signer() {
    hasher = new Hasher();
}

Signer::Signer(string appKey, string appSecret, std::string derivedAuthServiceName, std::string regionId) {
    mAppKey = appKey;
    mAppSecret = appSecret;
    this->derivedAuthServiceName = derivedAuthServiceName;
    this->regionId = regionId;
    hasher = new Hasher();
}

Signer::~Signer() {
    delete hasher;
}

void Signer::createSignature(HttpRequest &request) {
    time_t now = 0;
    time(&now);

    std::string dateTime = request.InitHeaders(now);

    vector<string> signedHeaders = getSignedHeaders(request.headers);

    std::string canonRequest = getCanonicalRequest(request, signedHeaders);
    LogPrint(LOG_LEVEL_DEBUG, "canonRequest [%s]", canonRequest.c_str());

    std::string info = generateInfoString(now);
    LogPrint(LOG_LEVEL_DEBUG, "info [%s]", info.c_str());

    std::string stringToSign = StringToSignDerived(canonRequest, info, now);
    LogPrint(LOG_LEVEL_DEBUG, "stringToSign [%s]", stringToSign.c_str());

    std::string derivedSk = GetDerivationKey(this->mAppKey, this->mAppSecret, info);
    LogPrint(LOG_LEVEL_DEBUG, "derivedSk [%s]", derivedSk.c_str());

    std::string signature = SignStringToSign(stringToSign, derivedSk);
    LogPrint(LOG_LEVEL_DEBUG, "signature [%s]", signature.c_str());

    std::string headerAuthorization = DerivationAuthHeaderValue(signature, this->mAppKey, info, signedHeaders);
    LogPrint(LOG_LEVEL_DEBUG, "headerAuthorization [%s]", headerAuthorization.c_str());

    request.headers[defaults::Authorization] = headerAuthorization;
}
// 	return fmt.Sprintf("%s Credential=%s/%s, SignedHeaders=%s, Signature=%s",
// 	    DerivationAlgorithm, accessKey, info, strings.Join(signedHeaders, ";"), signature)
std::string Signer::DerivationAuthHeaderValue(std::string signature, std::string accessKey,
                                      std::string info, std::vector<std::string> signedHeaders) {
    return string(DerivationAlgorithm) +
    " Credential=" + accessKey + "/" + info +
    ", SignedHeaders=" + fmtSignedHeaders(signedHeaders) +
    ", Signature=" + signature;
}

std::string Signer::SignStringToSign(string stringToSign, string signingKey) {
    unsigned int digestLength = SHA256_DIGEST_LENGTH;
    unsigned char * signatureChar = hasher->hmac((void*)signingKey.c_str(),
                                                 signingKey.size(),
                                                 stringToSign);
    std::string signature = hasher->hexEncode(signatureChar, digestLength);
    delete[] signatureChar;
    return signature;
}

string OpenSSL_HKDF1(string ak, string sk, string info) {
    const EVP_MD *md = EVP_sha256();
    EVP_PKEY_CTX *pCtx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);

    unsigned char out[SHA256_DIGEST_LENGTH+1];
    out[SHA256_DIGEST_LENGTH] = '\0';
    size_t outLen = SHA256_DIGEST_LENGTH;

    int ret = EVP_PKEY_derive_init(pCtx) <= 0
          || EVP_PKEY_CTX_hkdf_mode(pCtx, EVP_PKEY_HKDEF_MODE_EXTRACT_AND_EXPAND) <= 0
          || EVP_PKEY_CTX_set_hkdf_md(pCtx, md) <= 0
          || EVP_PKEY_CTX_set1_hkdf_salt(pCtx, ak.c_str(), ak.size()) <= 0
          || EVP_PKEY_CTX_set1_hkdf_key(pCtx, sk.c_str(), sk.size()) <= 0
          || EVP_PKEY_CTX_add1_hkdf_info(pCtx, info.c_str(), info.size()) <= 0
          || EVP_PKEY_derive(pCtx, out, &outLen) <= 0;

    EVP_PKEY_CTX_free(pCtx);
    if (ret == 0) {
        string outStr = string((char*)out, outLen);
        return EncodeToString(outStr);
    }

    return string();
}

std::string Signer::GetDerivationKey(std::string accessKey, std::string secretKey, std::string info) {
    return OpenSSL_HKDF1(accessKey, secretKey, info);
}

string Signer::generateInfoString(time_t now) {
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y%m%d", gmtime(&now));
    std::string str(buffer);

    return str + "/" + regionId + "/" + derivedAuthServiceName;
}

string Signer::StringToSignDerived(string canonicalRequest, string info, time_t now) {
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y%m%d", gmtime(&now));
    std::string str(buffer);

    return string(DerivationAlgorithm) + "\n" +
        str + "\n" +
        info + "\n" +
        getHexHash(canonicalRequest);
}

vector<string> Signer::getSignedHeaders(map<string, string> &headers) {
    vector<string> arr;

    for (auto it = headers.begin(); it != headers.end(); it++) {
        arr.push_back(toLowerCaseStr(it->first));
    }

    sort(arr.begin(), arr.end());

    return arr;
}

std::string Signer::getCanonicalQueryString(HttpRequest &request) {
    return "";
}

std::string Signer::fmtSignedHeaders(vector<string> &signedHeaders) {
    string result = "";

    for (int i = 0; i < signedHeaders.size(); i++) {
        result += toLowerCaseStr(signedHeaders[i]);
        if (i != signedHeaders.size() - 1) {
            result += ";";
        }
    }

    return result;
}

std::string Signer::getCanonicalHeaders(HttpRequest &request, std::vector<std::string> signedHeader) {
    string result = "";
    map<string, string> headers;

    for (auto &d: request.headers) {
        headers[toLowerCaseStr(d.first)] = d.second;
    }

    for (int i = 0; i < signedHeader.size(); i++) {
        string key = signedHeader[i];

        auto it = headers.find(key);
        if (it == headers.end()) {
            LogPrint(LOG_LEVEL_ERROR, "%s", "getCanonicalHeaders");
            break;
        }

        string token = key + ":" + it->second;
        result += token + "\n";
    }

    return result;
}

std::string Signer::getCanonicalRequest(HttpRequest &request, vector<string> signedHeaders) {
    string hexEncode = getHexHash(request.body);

    string result = request.method + "\n" +
            getCanonicalURI(request.urlPath) + "\n" +
            getCanonicalQueryString(request) + "\n" +
            getCanonicalHeaders(request, signedHeaders) + "\n" +
            fmtSignedHeaders(signedHeaders) + "\n" +
            hexEncode;

    return result;
}

std::string Signer::getCanonicalURI(std::string& uri) {
    if (uri.empty()) {
        return std::string("/");
    }
    if (uri[uri.length() - 1] != '/') {
        return  uriEncode(uri, true) + "/";

    }
    else {
        return uriEncode(uri, true);
    }
}

std::string Signer::getHexHash(std::string& payload) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    hasher->hashSHA256(payload, hash);
    return hasher->hexEncode(hash, SHA256_DIGEST_LENGTH);
}

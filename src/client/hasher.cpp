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
#include "hasher.h"
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <cstring>

Hasher::Hasher() {
}

const std::string Hasher::hexEncode(unsigned char* md, size_t length) {
    if (length <= 0) {
        return "";
    }
    char * buffer = new char[length * 2 + 1];

    for (size_t i = 0; i < length; i++) {
        sprintf(buffer + i * 2, "%02x", md[i]);
    }

    std::string str(buffer);
    delete[] buffer;
    return str;
}


int Hasher::hashSHA256(const std::string str, unsigned char *hash) {
    // Hash the input string
    char * cbuffer = new char[str.length() + 1];
    strcpy(cbuffer, str.c_str());

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, cbuffer, str.length());
    SHA256_Final(hash, &sha256);

    delete[] cbuffer;
    return SHA256_DIGEST_LENGTH;
}

unsigned char * Hasher::hmac(void* key, unsigned int keyLength, std::string data) {
    const EVP_MD * engine = EVP_sha256();
    unsigned int maxDigestLength = SHA256_DIGEST_LENGTH;

    unsigned char *md = new unsigned char[maxDigestLength];
    unsigned int mdLength;

    char * charData = new char[data.length()+1];
    strcpy(charData, data.c_str());

    HMAC(engine, key, keyLength, (unsigned char *)charData, strlen(charData), md, &mdLength);

    delete[] charData;
    return md;
}

int Hasher::hmac2(void* key, unsigned int keyLength,
                  char *data, unsigned int dataLen,
                  unsigned char* md, unsigned int *mdLength) {
    const EVP_MD * engine = EVP_sha256();
    unsigned int maxDigestLength = SHA256_DIGEST_LENGTH;

    char * charData = new char[dataLen+1];
    charData[dataLen] = '\0';
    memcpy(charData, data, dataLen);

    if (!HMAC(engine, key, keyLength,
         (unsigned char *)charData, dataLen,
         md, mdLength))
        return 0;

    delete[] charData;
    return 1;
}

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

#include "logger.h"
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <time.h>
#include <string>

const char* getCurrentFileName(const char* ch) {
    if (ch == NULL) {
        return NULL;
    }

    const char* pos = strrchr(ch, '/');
    if (pos != NULL) {
        return pos + 1;
    }

    return ch;
}

std::string currentTime() {
    // 获取当前时间
    time_t now;
    time(&now);

    // 将当前时间转换为tm结构
    struct tm* now_tm = localtime(&now);

    char format[256] = {0};

    // 格式化输出时间
    snprintf(format, sizeof format - 1, "%04d-%02d-%02d %02d:%02d:%02d",
             now_tm->tm_year + 1900, now_tm->tm_mon + 1, now_tm->tm_mday,
             now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);

    return std::string(format);
}

void LogPrint(LogLevel level, const char* file, int line, const char* fmt, ...) {
    if (level > LEVEL_WARN) {
        return;
    }

    char format[8192] = {0};
    va_list args;
    va_start(args, fmt);
    vsnprintf(format, sizeof format-1, fmt, args);
    va_end(args);

    const char* logLevel = "zero";

    char levelStr[5][32] = {"zero", "error", "warn", "info", "debug"};
    int index = level / 10;
    if (index < 5 && index >= 0) {
        logLevel = levelStr[index];
    }

    std::string t = currentTime();
    printf("%s [%s] %s:%d %s\n", t.c_str(), logLevel, file, line, format);
}
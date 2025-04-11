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

#ifndef LTS_SDK_LOGGER_H
#define LTS_SDK_LOGGER_H

enum LogLevel {
    LEVEL_ZERO = 0,  // no message
    LEVEL_ERROR = 10,  // some error message
    LEVEL_WARN  = 20,  // some warning
    LEVEL_INFO  = 30,  // normal information
    LEVEL_DEBUG = 40   // debug information
};

const char* getCurrentFileName(const char* ch);

#define LOG_FILENAME(x) getCurrentFileName((x))

#define LOG_LEVEL_INFO      LEVEL_INFO, LOG_FILENAME(__FILE__), __LINE__
#define LOG_LEVEL_WARN      LEVEL_WARN, LOG_FILENAME(__FILE__), __LINE__
#define LOG_LEVEL_ERROR     LEVEL_ERROR, LOG_FILENAME(__FILE__), __LINE__
#define LOG_LEVEL_DEBUG     LEVEL_DEBUG, LOG_FILENAME(__FILE__), __LINE__

void LogPrint(LogLevel level, const char* file, int line, const char* fmt, ...);

#endif //LTS_SDK_LOGGER_H

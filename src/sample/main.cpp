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
#include <cstdlib>
#include <getopt.h>

int main(int argc, char *argv[]) {
    Argument p;
    int c = '?';

    while((c = getopt(argc, argv, "e:a:s:r:p:l:k:v:g:m:n:q:t:h:")) != -1) {
        bool result = p.parseParameters(c, optarg);
        if (result) {
            continue;
        }

        switch(c) {
            case '?':
                std::cout << "Unknown option -" << optopt << "." << std::endl;
                return 1;
            default:
                std::cout << "Unknown option" << std::endl;
        }
    }

    if (!p.check()) {
        std::cout << "check failed" << std::endl;
        return 1;
    }

    TestSDK(p);

    return 0;
}

#!/bin/bash

ROOT=$(cd `dirname $0`; pwd)
cd ${ROOT}
mkdir -p opensource_build

cd ${ROOT}/opensource_build
if [ ! -d jsoncpp-install ] ; then
    cd ../opensource/jsoncpp;
    mkdir -p build; cd build;
    cmake ../ -DCMAKE_INSTALL_PREFIX=${ROOT}/opensource_build/jsoncpp-install -DCMAKE_CXX_FLAGS=-fPIC >/dev/null;
    make -j4 >/dev/null; make install;
fi

cd ${ROOT}/opensource_build
if [ ! -d zlib-install ] ; then
    cd ../opensource/zlib;
    CFLAGS="-O2 -fPIC" ./configure --prefix=${ROOT}/opensource_build/zlib-install --static >/dev/null;
    make -j4 >/dev/null; make install;
fi

cd ${ROOT}/opensource_build
if [ ! -d openssl-install ] ; then
    cd ../opensource/openssl;
    CFLAGS=-fPIC ./config --prefix=${ROOT}/opensource_build/openssl-install no-shared >/dev/null;
    make -j4 >/dev/null; make install_dev;
fi

cd ${ROOT}/opensource_build
if [ ! -d curl-install ] ; then
    cd ../opensource/curl;
    ./buildconf
    ./configure --prefix=${ROOT}/opensource_build/curl-install --with-ssl=${ROOT}/opensource_build/openssl-install --with-zlib=${ROOT}/opensource_build/zlib-install --disable-ldap --disable-shared CFLAGS=-fPIC >/dev/null;
    make -j4 >/dev/null; make install;
fi

# build agent
cd ${ROOT}
rm -rf build
mkdir -p build
cd build
cmake ../; make -j4


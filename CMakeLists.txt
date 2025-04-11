cmake_minimum_required(VERSION 2.8.11)

project(lts_sdk)

#set(CMAKE_CXX_FLAGS "-static-libstdc++ ${CMAKE_CXX_FLAGS}")

add_definitions(-DLINUX)

set(CMAKE_CXX_FLAGS "-g -fstack-protector-all -Wl,-z,noexecstack -Wl,-z,relro,-z,now -fPIC -fPIE -pie -Wl,--wrap=getentropy -std=c++11 ${CMAKE_CXX_FLAGS}")

#set(CMAKE_CXX_FLAGS "-g -fstack-protector-all -Wl,-z,noexecstack -Wl,-z,relro,-z,now -fPIC -fPIE -pie -D_FORTIFY_SOURCE=2 -O2 -DSHMEM -Wl,--wrap=getentropy ${CMAKE_CXX_FLAGS}")

#if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
#    add_definitions(-DBUILDX86)
#    set(CMAKE_CXX_FLAGS "-Wl,--wrap=memcpy,--wrap=secure_getenv ${CMAKE_CXX_FLAGS}")
#endif()

set(OPENSOURCE_INSTALL_DIR ${PROJECT_SOURCE_DIR}/opensource_build)

add_subdirectory(src/client)
add_subdirectory(src/sdk)
add_subdirectory(src/sample)

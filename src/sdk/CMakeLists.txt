aux_source_directory(. SDK)

include_directories(${OPENSOURCE_INSTALL_DIR}/jsoncpp-install/include/jsoncpp)
include_directories(${OPENSOURCE_INSTALL_DIR}/openssl-install/include)
include_directories(${OPENSOURCE_INSTALL_DIR}/curl-install/include)
include_directories(${PROJECT_SOURCE_DIR}/src/client)

link_directories(${OPENSOURCE_INSTALL_DIR}/jsoncpp-install/lib)
link_directories(${OPENSOURCE_INSTALL_DIR}/openssl-install/lib)
link_directories(${OPENSOURCE_INSTALL_DIR}/curl-install/lib)
link_directories(${OPENSOURCE_INSTALL_DIR}/zlib-install/lib)

add_library(sdk STATIC ${SDK})

#add_executable(main ${MAIN})

target_link_libraries(sdk client libjsoncpp.a libcurl.a libssl.a libcrypto.a libz.a pthread rt dl)

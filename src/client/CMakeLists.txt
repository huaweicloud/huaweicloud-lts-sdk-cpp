aux_source_directory(. CLIENT)

include_directories(${OPENSOURCE_INSTALL_DIR}/jsoncpp-install/include/jsoncpp)
include_directories(${OPENSOURCE_INSTALL_DIR}/openssl-install/include)
include_directories(${OPENSOURCE_INSTALL_DIR}/curl-install/include)

link_directories(${OPENSOURCE_INSTALL_DIR}/jsoncpp-install/lib)
link_directories(${OPENSOURCE_INSTALL_DIR}/openssl-install/lib)
link_directories(${OPENSOURCE_INSTALL_DIR}/curl-install/lib)
link_directories(${OPENSOURCE_INSTALL_DIR}/zlib-install/lib)

add_library(client STATIC ${CLIENT})

#add_executable(main ${CLIENT})

#target_link_libraries(main libjsoncpp.a libcurl.a libssl.a libcrypto.a libz.a pthread rt dl)

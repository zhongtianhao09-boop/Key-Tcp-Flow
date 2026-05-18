cmake_minimum_required(VERSION 3.20)

add_library(myinterface SHARED
    interface.cpp
    AesCrypto.cpp
    ../ShareMemory/SecKeyShm.cpp
    ../ShareMemory/BaseShm.cpp
    ../SecKeyNodeInfo/SecKeyNodeInfo.cpp
)

target_include_directories(myinterface PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(myinterface PUBLIC
    pthread
    crypto
)

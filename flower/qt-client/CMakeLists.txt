cmake_minimum_required(VERSION 3.20)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(flower-client
    main.cpp
    ClientWindow.cpp
    ../RsaCripto/RsaCripto.cpp
    ../tcp/TcpSocket.cpp
    ../codec/Codec.cpp
    ../codec/CodecFactory.cpp
    ../codec/RequestCodec.cpp
    ../codec/RequestFactory.cpp
    ../codec/RespondCodec.cpp
    ../codec/RespondFactory.cpp
    ../Myinterface/AesCrypto.cpp
    ../ShareMemory/SecKeyShm.cpp
    ../ShareMemory/BaseShm.cpp
    ${PROTO_SRCS} ${PROTO_HDRS}
)

target_include_directories(flower-client PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}
    ${Protobuf_INCLUDE_DIRS}
)

target_link_libraries(flower-client PRIVATE
    Qt6::Widgets
    ${Protobuf_LIBRARIES}
    pthread
    crypto
)

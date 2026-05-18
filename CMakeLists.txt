cmake_minimum_required(VERSION 3.20)
project(flower CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR})

# Oracle OCCI
set(ORACLE_HOME "/home/solid/oracle/instantclient_23_4")
set(OCCI_INCLUDE_DIR "${ORACLE_HOME}/sdk/include")
set(OCCI_LIB_DIR "${ORACLE_HOME}")
set(OCCI_LIB "${OCCI_LIB_DIR}/libocci.so" "${OCCI_LIB_DIR}/libclntsh.so")

include_directories(${OCCI_INCLUDE_DIR})
link_directories(${OCCI_LIB_DIR})

find_package(Protobuf REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS flower/proto/message.proto)

add_executable(test1 tests/test1.cpp flower/codec/Codec.cpp flower/codec/RequestCodec.cpp flower/codec/RespondCodec.cpp ${PROTO_SRCS} ${PROTO_HDRS})
add_executable(test_pool tests/test_pool.cpp flower/connectPool/ThreadPool/ThreadPool.cpp)
add_executable(test_tcp tests/test_tcp.cpp flower/tcp/TcpServer.cpp flower/tcp/TcpSocket.cpp)
add_executable(testocci tests/testocci.cpp)
	add_executable(query_db tests/query_db.cpp)
add_executable(test_sign tests/test_sign.cpp flower/RsaCripto/RsaCripto.cpp flower/codec/Codec.cpp flower/codec/RequestCodec.cpp flower/codec/RespondCodec.cpp flower/codec/CodecFactory.cpp flower/codec/RequestFactory.cpp flower/codec/RespondFactory.cpp ${PROTO_SRCS} ${PROTO_HDRS})
add_executable(test_agree tests/test_agree.cpp flower/RsaCripto/RsaCripto.cpp flower/codec/Codec.cpp flower/codec/RequestCodec.cpp flower/codec/RespondCodec.cpp flower/codec/CodecFactory.cpp flower/codec/RequestFactory.cpp flower/codec/RespondFactory.cpp flower/tcp/TcpSocket.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(testocci PRIVATE ${OCCI_LIB})
target_include_directories(test_sign PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${Protobuf_INCLUDE_DIRS})
	target_link_libraries(query_db PRIVATE ${OCCI_LIB})
target_link_libraries(test_sign PRIVATE ${Protobuf_LIBRARIES} pthread crypto)
target_include_directories(test_agree PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${Protobuf_INCLUDE_DIRS})
target_link_libraries(test_agree PRIVATE ${Protobuf_LIBRARIES} pthread crypto)

target_include_directories(test1 PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${Protobuf_INCLUDE_DIRS})
target_link_libraries(test1 PRIVATE ${Protobuf_LIBRARIES})

add_executable(test_shm flower/ShareMemory/main.cpp flower/ShareMemory/SecKeyShm.cpp flower/ShareMemory/BaseShm.cpp)
add_subdirectory(flower/Myinterface)
add_executable(myiface flower/Myinterface/main.cpp)
target_link_libraries(myiface PRIVATE myinterface)

target_link_libraries(test_shm PRIVATE pthread)
target_link_libraries(test_pool PRIVATE pthread)
target_link_libraries(test_tcp PRIVATE pthread)

add_executable(server
    flower/server/main.cpp
    flower/server/ServerOP.cpp
    flower/OCCIOP/OCCIOP.cpp
    flower/RsaCripto/RsaCripto.cpp
    flower/ShareMemory/SecKeyShm.cpp
    flower/ShareMemory/BaseShm.cpp
    flower/connectPool/ThreadPool/ThreadPool.cpp
    flower/connectPool/Logger/Logger.cpp
    flower/tcp/TcpServer.cpp
    flower/tcp/TcpSocket.cpp
    flower/codec/Codec.cpp
    flower/codec/CodecFactory.cpp
    flower/codec/RequestCodec.cpp
    flower/codec/RequestFactory.cpp
    flower/codec/RespondCodec.cpp
    flower/codec/RespondFactory.cpp
    flower/Myinterface/interface.cpp
    flower/Myinterface/AesCrypto.cpp
    ${PROTO_SRCS} ${PROTO_HDRS}
)
target_include_directories(server PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${Protobuf_INCLUDE_DIRS})
target_link_libraries(server PRIVATE ${Protobuf_LIBRARIES} pthread crypto ${OCCI_LIB})

add_executable(connectPool
    flower/connectPool/main.cpp
    flower/server/ServerOP.cpp
    flower/OCCIOP/OCCIOP.cpp
    flower/RsaCripto/RsaCripto.cpp
    flower/ShareMemory/SecKeyShm.cpp
    flower/ShareMemory/BaseShm.cpp
    flower/connectPool/SocketPool/SocketPool.cpp
    flower/connectPool/TaskQuene/TaskQuene.cpp
    flower/connectPool/ThreadPool/ThreadPool.cpp
    flower/connectPool/Logger/Logger.cpp
    flower/tcp/TcpServer.cpp
    flower/tcp/TcpSocket.cpp
    flower/codec/Codec.cpp
    flower/codec/CodecFactory.cpp
    flower/codec/RequestCodec.cpp
    flower/codec/RequestFactory.cpp
    flower/codec/RespondCodec.cpp
    flower/codec/RespondFactory.cpp
    flower/Myinterface/interface.cpp
    flower/Myinterface/AesCrypto.cpp
    ${PROTO_SRCS} ${PROTO_HDRS}
)
target_include_directories(connectPool PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${Protobuf_INCLUDE_DIRS})
target_link_libraries(connectPool PRIVATE ${Protobuf_LIBRARIES} pthread crypto ${OCCI_LIB})

add_executable(client
    flower/Client/main.cpp
    flower/Client/ClientOp.cpp
    flower/Myinterface/AesCrypto.cpp
    flower/RsaCripto/RsaCripto.cpp
    flower/tcp/TcpSocket.cpp
    flower/ShareMemory/SecKeyShm.cpp
    flower/ShareMemory/BaseShm.cpp
    flower/codec/Codec.cpp
    flower/codec/CodecFactory.cpp
    flower/codec/RequestCodec.cpp
    flower/codec/RequestFactory.cpp
    flower/codec/RespondCodec.cpp
    flower/codec/RespondFactory.cpp
    ${PROTO_SRCS} ${PROTO_HDRS}
)
target_include_directories(client PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${Protobuf_INCLUDE_DIRS})
target_link_libraries(client PRIVATE ${Protobuf_LIBRARIES} pthread crypto)

# Qt 管理界面
find_package(Qt6 COMPONENTS Widgets Sql)
add_subdirectory(flower/qt-server)
add_subdirectory(flower/qt-client)

cmake_minimum_required(VERSION 3.20)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(OCCI_INCLUDE_DIR "/home/solid/oracle/instantclient_23_4/sdk/include")
set(OCCI_LIB_DIR "/home/solid/oracle/instantclient_23_4")
set(OCCI_LIB "${OCCI_LIB_DIR}/libocci.so" "${OCCI_LIB_DIR}/libclntsh.so")

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${OCCI_INCLUDE_DIR})
link_directories(${OCCI_LIB_DIR})

add_executable(flower-qt
    main.cpp
    MainWindow.cpp
    ../OCCIOP/OCCIOP.cpp
)

target_link_libraries(flower-qt PRIVATE
    Qt6::Widgets
    Qt6::Sql
    ${OCCI_LIB}
    pthread
)

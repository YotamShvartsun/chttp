cmake_minimum_required(VERSION 3.9)
project(chttp VERSION 1.0 DESCRIPTION "Http server framework written in c++")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
find_package(Threads)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
include(GNUInstallDirs)

include_directories(include)
include_directories(include/chttp)
include_directories(include/chttp/util)
include_directories(include/chttp/data)


add_library(chttp SHARED
        src/chttp/Threadpool/Threadpool.cpp
        src/chttp/data/GetRequest.cpp
        src/chttp/data/HttpRequest.cpp
        src/chttp/data/HttpResponse.cpp
        src/chttp/data/PostRequest.cpp
        src/chttp/util/MultipartField.cpp
        src/chttp/util/Socket.cpp
        src/chttp/util/Url.cpp
        src/chttp/RequestHandler.cpp
        src/chttp/Router.cpp
        src/chttp/Server.cpp
        include/chttp/data/Cookie.hpp)

target_include_directories(chttp PRIVATE include)
target_include_directories(chttp PRIVATE include/chttp)
target_include_directories(chttp PRIVATE include/chttp/util)
target_include_directories(chttp PRIVATE include/chttp/data)
target_include_directories(chttp PRIVATE include/Threapool)
target_include_directories(chttp PRIVATE src)
set_target_properties(chttp PROPERTIES VERSION ${PROJECT_VERSION})

configure_file(chttp.pc.in chttp.pc @ONLY)

install(TARGETS chttp
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_BINARY_DIR}/chttp.pc
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)


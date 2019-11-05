cmake_minimum_required(VERSION 3.9)
project(chttp)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

include_directories(include)
include_directories(include/chttp)
include_directories(include/chttp/data)
include_directories(include/chttp/util)
include_directories(include/Threapool)

add_library(Threadpool include/Threapool/Threadpool.h src/Threadpool/Threadpool.cpp)
add_library(chttp include/chttp/data/GetRequest.h
        include/chttp/data/HttpRequest.h
        include/chttp/data/HttpResponse.h
        include/chttp/data/PostRequest.h
        include/chttp/util/File.h
        include/chttp/util/Socket.h
        include/chttp/util/Url.h
        include/chttp/Handler.h
        include/chttp/Router.h
        include/chttp/Server.h
        include/Threapool/Threadpool.h
        src/chttp/data/GetRequest.cpp
        src/chttp/data/HttpRequest.cpp
        src/chttp/data/HttpResponse.cpp
        src/chttp/data/PostRequest.cpp
        src/chttp/util/File.cpp
        src/chttp/util/Socket.cpp
        src/chttp/util/Url.cpp
        src/chttp/Handler.cpp
        src/chttp/Router.cpp
        src/chttp/Server.cpp)
target_link_libraries(chttp Threadpool)
add_executable(chttpTest
        src/test.cpp)
target_link_libraries(chttpTest chttp)
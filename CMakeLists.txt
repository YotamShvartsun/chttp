cmake_minimum_required(VERSION 3.9)

project(Chttp LANGUAGES C CXX)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++17 -pthread")
find_package(Threads)

add_library(threadlib STATIC src/Threadpool/*.cpp include/Threadpool/*.h)
add_library(Chttp STATIC src/chttp/*.cpp include/chttp/*.h)
target_include_directories(threadlib PUBLIC include/Threadpool)
target_include_directories(Chttp PUBLIC include/chttp)
target_compile_features(threadlib PUBLIC cxx_std_17)
target_compile_features(Chttp PUBLIC cxx_std_17)
add_executable(ChttpTest src/test.cpp)
target_link_libraries(ChttpTest PUBLIC threadlib Chttp)
cmake_minimum_required(VERSION 3.9)
project(examples)
find_library(chttp REQUIRED)
set(CMAKE_CXX_STANDARD 14)

add_executable(examples main.cpp)

target_link_libraries(examples chttp)
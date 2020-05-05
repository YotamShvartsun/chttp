cmake_minimum_required(VERSION 3.9)
project(examples)
find_library(CHTTP NAMES chttp)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/SQLiteCpp)
include_directories(${CMAKE_CURRENT_LIST_DIR}/thirdparty/SQLiteCpp/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/thirdparty/json)
include_directories(${chttp_INCLUDE_DIRS})

add_executable(examples main.cpp)

target_link_libraries(examples
        SQLiteCpp
        chttp)
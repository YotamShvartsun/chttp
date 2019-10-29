cmake_minimum_required(VERSION 3.9)
project(Chttp)
add_definitions(-DDEBUG)
include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(ChttpTest ${SOURCES})
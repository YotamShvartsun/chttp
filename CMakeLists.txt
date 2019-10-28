cmake_minimum_required(VERSION 3.9)
project(Chttp)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(ChttpTest ${SOURCES})
cmake_minimum_required(VERSION 3.8)
project(Server)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c)
link_libraries(ws2_32.lib)
add_executable(Server ${SOURCE_FILES})
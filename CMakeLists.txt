cmake_minimum_required(VERSION 3.6)
project(threads_part2)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES main.cpp)
add_executable(threads_part2 ${SOURCE_FILES})
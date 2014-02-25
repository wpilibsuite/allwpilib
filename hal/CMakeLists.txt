cmake_minimum_required(VERSION 2.8)
project(HAL)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
file(GLOB_RECURSE SRC_FILES Athena/src/*.cpp)
include_directories(Athena/src/main/native include/src/main/include)
add_library(HALAthena SHARED ${SRC_FILES})
# lib/ c m gcc_s ld-linux
# usr/lib stdc++

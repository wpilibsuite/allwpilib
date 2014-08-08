cmake_minimum_required(VERSION 2.8)
project(WPILibC++)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
include_directories(include/ ${NWT_API_INCLUDES} ${HAL_API_INCLUDES})
add_library(WPILib STATIC ${SRC_FILES})
set(CMAKE_CXX_FLAGS "-fPIC")
target_link_libraries(WPILib)
INSTALL(TARGETS WPILib ARCHIVE DESTINATION src COMPONENT src)
INSTALL(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT headers)
# lib/ c m gcc_s ld-linux
# usr/lib stdc++
# ni_emb
# HAL NWT

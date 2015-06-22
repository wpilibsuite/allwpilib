cmake_minimum_required(VERSION 2.8)
project(NetworkTables)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wformat=2 -Wall -Wextra -Werror -pedantic -Wno-unused-parameter")

file(GLOB_RECURSE SRC_TARGET_FILES src/*.cpp)
#file(GLOB_RECURSE SRC_SHARE_FILES lib/share/*.cpp)
include_directories(include src)
add_library(ntcore STATIC ${SRC_SHARE_FILES} ${SRC_TARGET_FILES})
target_link_libraries(ntcore)
INSTALL(TARGETS ntcore ARCHIVE DESTINATION lib COMPONENT lib)
INSTALL(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT headers)
# lib/ c gcc_s ld-linux
# usr/lib stdc++
# NiRioSrv

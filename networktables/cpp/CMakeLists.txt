cmake_minimum_required(VERSION 2.8)
project(NetworkTables)

file(GLOB_RECURSE SRC_TARGET_FILES lib/Athena/*.cpp)
file(GLOB_RECURSE SRC_SHARE_FILES lib/share/*.cpp)
include_directories(include)
add_library(NetworkTables STATIC ${SRC_SHARE_FILES} ${SRC_TARGET_FILES})
target_link_libraries(NetworkTables ${NI_LIBS})
INSTALL(TARGETS NetworkTables ARCHIVE DESTINATION lib COMPONENT lib)
INSTALL(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT headers)
# lib/ c gcc_s ld-linux
# usr/lib stdc++
# NiRioSrv

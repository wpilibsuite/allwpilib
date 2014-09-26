cmake_minimum_required(VERSION 2.8)
project(WPILibC++Devices)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
include_directories(include/ ${WPILIB_INCLUDES} ${HAL_API_INCLUDES} ${NWT_API_INCLUDES})
add_library(wpilib_nonshared STATIC ${SRC_FILES} ${COM_SRC_FILES})
target_link_libraries(wpilib_nonshared HALAthena NetworkTables ${NI_LIBS})
INSTALL(TARGETS wpilib_nonshared ARCHIVE DESTINATION lib COMPONENT lib)
INSTALL(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT headers)
# lib/ c m gcc_s ld-linux
# usr/lib stdc++
# ni_emb
# HAL NWT

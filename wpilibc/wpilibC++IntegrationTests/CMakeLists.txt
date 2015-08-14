cmake_minimum_required(VERSION 2.8)
project(WPILibC++IntegrationTests)

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,/opt/GenICam_v2_3/bin/Linux_armv7-a")

file(GLOB_RECURSE SRC_FILES src/*.cpp src/gtest/src/gtest-all.cc src/gtest/src/gtest_main.cc)
include_directories(include/ src/gtest/ src/gtest/include/ ../wpilibC++Devices/include/ ${WPILIB_INCLUDES} ${HAL_API_INCLUDES} ${NWT_API_INCLUDES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wno-unused-variable")
add_executable(FRCUserProgram ${SRC_FILES})
target_link_libraries(FRCUserProgram wpilib_nonshared HALAthena ntcore ${NI_LIBS})

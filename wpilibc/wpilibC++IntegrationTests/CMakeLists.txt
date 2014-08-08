cmake_minimum_required(VERSION 2.8)
project(WPILibC++IntegrationTests)

file(GLOB_RECURSE SRC_FILES src/*.cpp src/gtest/src/gtest-all.cc src/gtest/src/gtest_main.cc)
include_directories(include/ src/gtest/ src/gtest/include/ ../wpilibC++Devices/include/ ${WPILIB_INCLUDES} ${HAL_API_INCLUDES} ${NWT_API_INCLUDES})
add_executable(FRCUserProgram ${SRC_FILES})
target_link_libraries(FRCUserProgram WPILib WPILibAthena WPILib HALAthena NetworkTables ${NI_LIBS})


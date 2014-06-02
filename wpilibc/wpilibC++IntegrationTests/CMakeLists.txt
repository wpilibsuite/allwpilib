cmake_minimum_required(VERSION 2.8)
project(WPILibC++IntegrationTests)

file(GLOB_RECURSE SRC_FILES src/*.cpp src/gtest/gtest-all.cc)
include_directories(include/ src/gtest/ src/gtest/include/ ../wpilibC++/include/ ${HAL_API_INCLUDES} ${NWT_API_INCLUDES})
add_executable(FRCUserProgram ${SRC_FILES})
target_link_libraries(FRCUserProgram WPILibAthena HALAthena NetworkTables ${NI_LIBS})


cmake_minimum_required(VERSION 2.8)
project(HAL)

file(GLOB_RECURSE SRC_FILES lib/Athena/*.cpp)
include_directories(lib/Athena lib/Athena/FRC_FPGA_ChipObject include)
add_library(HALAthena STATIC ${SRC_FILES})
target_link_libraries(HALAthena ${NI_LIBS})
INSTALL(TARGETS HALAthena ARCHIVE DESTINATION lib COMPONENT lib)
INSTALL(FILES ${NI_LIBS} ${WPI_LD_LIBS} DESTINATION lib COMPONENT ni_lib)
INSTALL(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT headers)

add_library(HALAthena_shared SHARED ${SRC_FILES})
target_link_libraries(HALAthena_shared ${NI_LIBS})
INSTALL(TARGETS HALAthena_shared LIBRARY DESTINATION lib COMPONENT lib)
# lib/ c m gcc_s ld-linux
# usr/lib 
# FRC_NetworkCommunication FRC_FPGA_ChipObject RoboRIO_FRC_ChipObject

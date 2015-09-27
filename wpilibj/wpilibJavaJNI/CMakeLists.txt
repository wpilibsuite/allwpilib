cmake_minimum_required(VERSION 2.8)
project(WPILibJavaJNI)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat=2 -Wextra -Wno-unused-parameter -fPIC -Werror")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,/usr/local/frc/rpath-lib")
SET(CMAKE_SKIP_BUILD_RPATH TRUE)
file(GLOB_RECURSE NI_LIBS ../../ni-libraries/*.so)
list(REMOVE_ITEM NI_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/../../ni-libraries/libwpi.so ${CMAKE_CURRENT_SOURCE_DIR}/../../ni-libraries/libwpi_2015.so)
get_filename_component(HAL_API_INCLUDES ../../hal/include REALPATH)
## depends on libFRC_NetComm, halAthena Hal incl, wpilibJava (jar)
# this file requires embeddedJDKHome to be set
# all the h files must be generated, in target/include

file(GLOB_RECURSE SRC_FILES lib/*.cpp)
include_directories(target/include ${embeddedJDKHome}/include ${embeddedJDKHome}/include/linux ${HAL_API_INCLUDES})
add_library(wpilibJavaJNI SHARED ${SRC_FILES})
find_library(HAL_LIB libHALAthena.a target/lib)
add_library(HALAthena UNKNOWN IMPORTED)
set_property(TARGET HALAthena PROPERTY IMPORTED_LOCATION ${HAL_LIB})
target_link_libraries(wpilibJavaJNI HALAthena ${NI_LIBS})
INSTALL(TARGETS wpilibJavaJNI LIBRARY DESTINATION target)
# lib/ c m gcc_s ld-linux
# usr/lib stdc++
# FRC_NetworkCommunication NiFpgaLv RoboRIO_FRC_ChipObject


cmake_minimum_required(VERSION 2.8)
project(WPILibSim)

include (FindPkgConfig)
if (PKG_CONFIG_FOUND)
  pkg_check_modules(GAZEBO gazebo)
endif()

find_package(gazebo REQUIRED)

file(GLOB_RECURSE SRC_FILES src/*.cpp $ENV{ALLWPILIB}/networktables/cpp/lib/share/*.cpp
                                      $ENV{ALLWPILIB}/networktables/cpp/lib/Athena/*.cpp
                                      $ENV{ALLWPILIB}/hal/lib/Athena/Task.cpp
                                      $ENV{ALLWPILIB}/hal/lib/Athena/Semaphore.cpp
                                      $ENV{ALLWPILIB}/hal/lib/Athena/cpp/*.cpp)
include_directories(include/ $ENV{ALLWPILIB}/networktables/cpp/include
                    $ENV{ALLWPILIB}/hal/include
                    ${Boost_INCLUDE_DIR} ${GAZEBO_INCLUDE_DIRS}) # ${NWT_API_INCLUDES}
add_library(WPILibSim SHARED ${SRC_FILES})
target_link_libraries(WPILibSim  ${GAZEBO_LIBRARIES}) # NetworkTables
INSTALL(TARGETS WPILibSim LIBRARY DESTINATION src COMPONENT src)
INSTALL(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT headers)
# ni_emb
# HAL NWT

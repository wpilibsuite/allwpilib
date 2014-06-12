cmake_minimum_required(VERSION 2.8.3)
project(frc_gazebo_plugin)

include (FindPkgConfig)
if (PKG_CONFIG_FOUND)
  pkg_check_modules(GAZEBO gazebo)
endif()

find_package(gazebo REQUIRED)

file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.cc)
include_directories(src ${Boost_INCLUDE_DIR} ${GAZEBO_INCLUDE_DIRS})
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

link_directories(${GAZEBO_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} ${GAZEBO_LIBRARIES} tinyxml2)
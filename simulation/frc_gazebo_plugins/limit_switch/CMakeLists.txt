cmake_minimum_required(VERSION 2.8.3)
project(gz_limit_switch)

include (FindPkgConfig)
if (PKG_CONFIG_FOUND)
  pkg_check_modules(GAZEBO gazebo)
endif()

find_package(gazebo REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_library(GZ_MSGS NAMES gz_msgs PATHS ../msgs/build)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
include_directories(src ${Boost_INCLUDE_DIR} ${GAZEBO_INCLUDE_DIRS} ../msgs/src)
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

link_directories(${GAZEBO_LIBRARY_DIRS} ../msgs/build/)
target_link_libraries(${PROJECT_NAME} ${GZ_MSGS} ${GAZEBO_LIBRARIES} ${Boost_LIBRARIES})

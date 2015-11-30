cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0015 NEW)
project(wpilibcSim)

get_filename_component(HAL_API_INCLUDES ../../hal/include REALPATH)

file(GLOB_RECURSE COM_SRC_FILES ../shared/src/*.cpp
  src/*.cpp)

set (INCLUDE_FOLDERS include
  ../shared/include
  ../../hal/include
  ${NTCORE_INCLUDE_DIR}
  ${GZ_MSGS_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${GAZEBO_INCLUDE_DIRS})

message("inc=${INCLUDE_FOLDERS}")

include_directories(${INCLUDE_FOLDERS})

link_directories(${NTCORE_LIBDIR})

add_library(${PROJECT_NAME} SHARED ${SRC_FILES} ${COM_SRC_FILES})

target_link_libraries(${PROJECT_NAME} gz_msgs ntcore)

cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0015 NEW)
project(wpilibcSim)

file(GLOB_RECURSE COM_SRC_FILES ../shared/src/*.cpp
  src/*.cpp)

add_definitions(-DNAMESPACED_WPILIB)

set (INCLUDE_FOLDERS include
  ../shared/include
  ../../hal/include
  ${NTCORE_INCLUDE_DIR}
  ${WPIUTIL_INCLUDE_DIR}
  ${GZ_MSGS_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${GAZEBO_INCLUDE_DIRS})

include_directories(${INCLUDE_FOLDERS})

link_directories(${NTCORE_LIBDIR} ${WPIUTIL_LIBDIR})

add_library(${PROJECT_NAME} SHARED ${SRC_FILES} ${COM_SRC_FILES})

target_link_libraries(${PROJECT_NAME} ntcore wpiutil)

set_target_properties(${PROJECT_NAME}
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${SIMULATION_INSTALL_DIR}/lib)

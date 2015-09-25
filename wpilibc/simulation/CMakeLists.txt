cmake_minimum_required(VERSION 2.8)
project(WPILibSim)

if (WIN32)
  #temporary until we build dlls
  add_definitions(-DBUILDING_STATIC_LIBS)

  # XXX: should be set via CMake variables in configure.bat
  set(PTHREAD_INCLUDE_DIR "C:/Users/peter/gz-ws/pthread-w32/include")
  set(PTHREAD_LIBRARY "C:/Users/peter/gz-ws/pthread-w32/libs/x64/pthreadVC2.lib")
endif()

get_filename_component(HAL_API_INCLUDES ../../hal/include REALPATH)
get_filename_component(NWT_API_INCLUDES ../../networktables/cpp/include REALPATH)


# also on windows use sprintf_s instead of snprintf
# TODO: find a more permenenant solution
if (WIN32)
  add_definitions(-Dsnprintf=sprintf_s)
endif()

file(GLOB_RECURSE COM_SRC_FILES ../wpilibC++/src/*.cpp)


set (INCLUDE_FOLDERS include
  ../wpilibC++/include
  ../../networktables/ntcore/include
  ../../hal/include
  ${GZ_MSGS_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${GAZEBO_INCLUDE_DIRS})

if (WIN32)
  #these paths will be fixed when a more permenant windows development solution is found
  set(INCLUDE_FOLDERS ${INCLUDE_FOLDERS}
    C:/Users/peter/gz-ws/protobuf-2.6.0-win64-vc12/src
    C:/Users/peter/gz-ws/sdformat/src/win/tinyxml
    C:/Users/peter/gz-ws/FreeImage-vc12-x64-release-debug/Source
    C:/Users/peter/gz-ws/tbb43_20141023oss/include
    ${PTHREAD_INCLUDE_DIR})
endif()

include_directories(${INCLUDE_FOLDERS})

link_directories(${GAZEBO_LIBRARY_DIRS})

if (WIN32)
  add_library(WPILibSim ${SRC_FILES} ${COM_SRC_FILES})
else()
  add_library(WPILibSim SHARED ${SRC_FILES} ${COM_SRC_FILES})
endif()

target_link_libraries(WPILibSim gz_msgs ntcore ${PTHREAD_LIBRARY} ${Boost_LIBRARIES} ${GAZEBO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}  -fPIC) # NetworkTables

if (WIN32)
  set_target_properties(${project}  PROPERTIES LINK_FLAGS "/DEBUG")
endif()

#copy to eclipse plugin

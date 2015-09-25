cmake_minimum_required(VERSION 2.8)
project(AllC++Sim)

include(CheckCXXCompilerFlag)
include (FindPkgConfig)
include(GNUInstallDirs)

#copied from GazeboUtils.cmake
macro (APPEND_TO_CACHED_STRING _string _cacheDesc)
  FOREACH (newItem ${ARGN})
    SET (${_string} "${${_string}} ${newItem}" CACHE INTERNAL ${_cacheDesc} FORCE)
  ENDFOREACH (newItem ${ARGN})
endmacro (APPEND_TO_CACHED_STRING)

#check for depenedencies
find_package(gazebo REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(Protobuf REQUIRED)

if (NOT PROTOBUF_FOUND)
	MESSAGE ("Missing: Google Protobuf (libprotobuf-dev)")
endif()
if (NOT PROTOBUF_PROTOC_EXECUTABLE)
	MESSAGE ( "Missing: Google Protobuf Compiler (protobuf-compiler)")
endif()
if (NOT PROTOBUF_PROTOC_LIBRARY)
	MESSAGE ("Missing: Google Protobuf Compiler Library (libprotoc-dev)")
endif()

#on windows we produce .dlls with no prefix
if(WIN32)
  #allows us to define constexpr and noexcept in macros
  #since msvc 2013 doesn't support them
  add_definitions(-D_ALLOW_KEYWORD_MACROS)

  # defines things like M_PI
  add_definitions(-D_USE_MATH_DEFINES)

  # get rid of min max macros on windows
  add_definitions(-DNOMINMAX)

  add_definitions(-DWIN32_LEAN_AND_MEAN)

  SET(CMAKE_FIND_LIBRARY_PREFIXES "")
  SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
endif()

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFRC_SIMULATOR /MDd /Zi")
else ()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -DFRC_SIMULATOR -Wno-unused-parameter -pthread -fPIC -fpermissive")
endif()


include_directories("build")
add_subdirectory(simulation/gz_msgs)
add_subdirectory(wpilibc/simulation)
add_subdirectory(simulation/frc_gazebo_plugins)
add_subdirectory(ntcore)

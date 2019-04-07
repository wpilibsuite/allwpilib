# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()


project(allwpilib)
cmake_minimum_required(VERSION 3.3.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

INCLUDE(CPack)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND MSVC)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Default install dir on windows" FORCE)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_JAVA_TARGET_OUTPUT_DIR ${CMAKE_BINARY_DIR}/jar)

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/wpilib/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/wpilib/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/wpilib/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")

option(WITHOUT_JAVA "don't include java and JNI in the build" OFF)
option(BUILD_SHARED_LIBS "build with shared libs (needed for JNI)" ON)
option(WITHOUT_CSCORE "Don't build cscore (removes OpenCV requirement)" OFF)
option(WITHOUT_ALLWPILIB "Don't build allwpilib (removes OpenCV requirement)" ON)
option(WITHOUT_HAL "Don't build HAL" ON)
option(USE_EXTERNAL_HAL "Use a separately built HAL" OFF)
set(EXTERNAL_HAL_FILE "" CACHE FILEPATH "Location to look for an external HAL CMake File")

if (NOT WITHOUT_JAVA AND NOT BUILD_SHARED_LIBS)
    message(FATAL_ERROR "
FATAL: Cannot build static libs with Java enabled.
       Static libs requires both BUILD_SHARED_LIBS=OFF and
       WITHOUT_JAVA=ON
")
endif()

set( wpilib_dest wpilib)
set( include_dest wpilib/include )
set( main_lib_dest wpilib/lib )
set( java_lib_dest wpilib/java )
set( jni_lib_dest wpilib/jni )

if (MSVC)
    set (wpilib_config_dir ${wpilib_dest})
else()
    set (wpilib_config_dir share/wpilib)
endif()

add_subdirectory(wpiutil)
add_subdirectory(ntcore)

if (NOT WITHOUT_HAL)
    add_subdirectory(hal)
    set (HAL_DEP_REPLACE "find_dependency(hal)")
endif()

if (NOT WITHOUT_CSCORE)
    add_subdirectory(cscore)
    add_subdirectory(cameraserver)
    set (CSCORE_DEP_REPLACE "find_dependency(cscore)")
    set (CAMERASERVER_DEP_REPLACE "find_dependency(cameraserver)")
    if (NOT WITHOUT_ALLWPILIB AND NOT WITHOUT_HAL)
        add_subdirectory(wpilibj)
        add_subdirectory(wpilibc)
        set (WPILIBC_DEP_REPLACE "find_dependency(wpilibc)")
    endif()
endif()

configure_file(wpilib-config.cmake.in ${CMAKE_BINARY_DIR}/wpilib-config.cmake )
install(FILES ${CMAKE_BINARY_DIR}/wpilib-config.cmake DESTINATION ${wpilib_config_dir})

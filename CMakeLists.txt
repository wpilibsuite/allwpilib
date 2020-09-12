# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL " ${CMAKE_CURRENT_BINARY_DIR}")
  message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()


project(allwpilib)
cmake_minimum_required(VERSION 3.3.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

set(WPILIB_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

INCLUDE(CPack)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${WPILIB_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${WPILIB_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${WPILIB_BINARY_DIR}/bin)
set(CMAKE_JAVA_TARGET_OUTPUT_DIR ${WPILIB_BINARY_DIR}/jar)

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

# Options for building certain parts of the repo. Everything is built by default.
option(BUILD_SHARED_LIBS "Build with shared libs (needed for JNI)" ON)
option(WITH_JAVA "Include java and JNI in the build" ON)
option(WITH_CSCORE "Build cscore (needs OpenCV)" ON)
option(WITH_WPIMATH "Build wpimath" ON)
option(WITH_WPILIB "Build hal, wpilibc/j, and myRobot (needs OpenCV)" ON)
option(WITH_OLD_COMMANDS "Build old commands" OFF)
option(WITH_EXAMPLES "Build examples" OFF)
option(WITH_TESTS "Build unit tests (requires internet connection)" ON)
option(WITH_GUI "Build GUI items" ON)
option(WITH_SIMULATION_MODULES "Build simulation modules" ON)

# Options for external HAL.
option(WITH_EXTERNAL_HAL "Use a separately built HAL" OFF)
set(EXTERNAL_HAL_FILE "" CACHE FILEPATH "Location to look for an external HAL CMake File")

# Options for using a package manager (vcpkg) for certain dependencies.
option(USE_VCPKG_LIBUV "Use vcpkg libuv" OFF)
option(USE_VCPKG_EIGEN "Use vcpkg eigen" OFF)

# Options for installation.
option(WITH_FLAT_INSTALL "Use a flat install directory" OFF)

# Options for location of OpenCV Java.
set(OPENCV_JAVA_INSTALL_DIR "" CACHE PATH "Location to search for the OpenCV jar file")

# Set default build type to release with debug info (i.e. release mode optimizations
# are performed, but debug info still exists).
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
endif()

# We always want flat install with MSVC.
if (MSVC)
  set(WITH_FLAT_INSTALL ON)
endif()

if (WITH_JAVA AND NOT BUILD_SHARED_LIBS)
    message(FATAL_ERROR "
FATAL: Cannot build static libs with Java enabled.
       Static libs requires both BUILD_SHARED_LIBS=OFF and
       WITH_JAVA=OFF
")
endif()

if (WITH_SIMULATION_MODULES AND NOT BUILD_SHARED_LIBS)
    message(FATAL_ERROR "
FATAL: Cannot build static libs with simulation modules enabled.
       Static libs requires both BUILD_SHARED_LIBS=OFF and
       WITH_SIMULATION_MODULES=OFF
")
endif()

if (NOT WITH_JAVA OR NOT WITH_CSCORE)
    if(NOT "${OPENCV_JAVA_INSTALL_DIR}" STREQUAL "")
        message(WARNING "
WARNING: OpenCV Java dir set but java is not enabled!
It will be ignored.
")
    endif()
endif()

if (NOT WITH_WPILIB AND WITH_SIMULATION_MODULES)
    message(FATAL_ERROR "
FATAL: Cannot build simulation modules with wpilib disabled.
       Enable wpilib by setting WITH_WPILIB=ON
")
endif()

if (NOT WITH_WPIMATH AND WITH_WPILIB)
    message(FATAL_ERROR "
FATAL: Cannot build wpilib without wpimath.
       Enable wpimath by setting WITH_WPIMATH=ON
")
endif()

if (NOT WITH_OLD_COMMANDS AND WITH_EXAMPLES)
    message(FATAL_ERROR "
FATAL: Cannot build examples with old commands disabled.
       Enable old commands by setting WITH_OLD_COMMANDS=ON
")
endif()

set( wpilib_dest wpilib)
set( include_dest wpilib/include )
set( main_lib_dest wpilib/lib )
set( java_lib_dest wpilib/java )
set( jni_lib_dest wpilib/jni )

if (WITH_FLAT_INSTALL)
    set (wpilib_config_dir ${wpilib_dest})
else()
    set (wpilib_config_dir share/wpilib)
endif()

if (USE_VCPKG_LIBUV)
set (LIBUV_VCPKG_REPLACE "find_package(unofficial-libuv CONFIG)")
endif()

if (USE_VCPKG_EIGEN)
set (EIGEN_VCPKG_REPLACE "find_package(Eigen3 CONFIG)")
endif()

if (WITH_FLAT_INSTALL)
set(WPIUTIL_DEP_REPLACE "include($\{SELF_DIR\}/wpiutil-config.cmake)")
set(NTCORE_DEP_REPLACE "include($\{SELF_DIR\}/ntcore-config.cmake)")
set(CSCORE_DEP_REPLACE_IMPL "include(\${SELF_DIR}/cscore-config.cmake)")
set(CAMERASERVER_DEP_REPLACE_IMPL "include(\${SELF_DIR}/cameraserver-config.cmake)")
set(HAL_DEP_REPLACE_IMPL "include(\${SELF_DIR}/hal-config.cmake)")
set(WPIMATH_DEP_REPLACE "include($\{SELF_DIR\}/wpimath-config.cmake)")
set(WPILIBC_DEP_REPLACE_IMPL "include(\${SELF_DIR}/wpilibc-config.cmake)")
set(WPILIBNEWCOMMANDS_DEP_REPLACE "include(\${SELF_DIR}/wpilibNewcommands-config.cmake)")
set(WPILIBOLDCOMMANDS_DEP_REPLACE "include(\${SELF_DIR}/wpilibOldcommands-config.cmake)")
else()
set(WPIUTIL_DEP_REPLACE "find_dependency(wpiutil)")
set(NTCORE_DEP_REPLACE "find_dependency(ntcore)")
set(CSCORE_DEP_REPLACE_IMPL "find_dependency(cscore)")
set(CAMERASERVER_DEP_REPLACE_IMPL "find_dependency(cameraserver)")
set(HAL_DEP_REPLACE_IMPL "find_dependency(hal)")
set(WPIMATH_DEP_REPLACE "find_dependency(wpimath)")
set(WPILIBC_DEP_REPLACE_IMPL "find_dependency(wpilibc)")
set(WPILIBNEWCOMMANDS_DEP_REPLACE "find_dependency(wpilibNewCommands)")
set(WPILIBOLDCOMMANDS_DEP_REPLACE "find_dependency(wpilibOldCommands)")
endif()

set(FILENAME_DEP_REPLACE "get_filename_component(SELF_DIR \"$\{CMAKE_CURRENT_LIST_FILE\}\" PATH)")
set(SELF_DIR "$\{SELF_DIR\}")

if (WITH_TESTS)
    enable_testing()
    add_subdirectory(googletest)
    include(GoogleTest)
endif()

add_subdirectory(wpiutil)
add_subdirectory(ntcore)

if (WITH_WPIMATH)
    add_subdirectory(wpimath)
endif()

if (WITH_GUI)
    add_subdirectory(imgui)
    add_subdirectory(wpigui)
    add_subdirectory(glass)
endif()

if (WITH_CSCORE)
    set(CSCORE_DEP_REPLACE ${CSCORE_DEP_REPLACE_IMPL})
    set(CAMERASERVER_DEP_REPLACE ${CAMERASERVER_DEP_REPLACE_IMPL})
    add_subdirectory(cscore)
    add_subdirectory(cameraserver)
    if (WITH_WPILIB)
        set(HAL_DEP_REPLACE ${HAL_DEP_REPLACE_IMPL})
        set(WPILIBC_DEP_REPLACE ${WPILIBC_DEP_REPLACE_IMPL})
        add_subdirectory(hal)
        add_subdirectory(wpilibj)
        add_subdirectory(wpilibc)
        add_subdirectory(wpilibNewCommands)
        if (WITH_OLD_COMMANDS)
            add_subdirectory(wpilibOldCommands)
        endif()
        if (WITH_EXAMPLES)
            add_subdirectory(wpilibcExamples)
        endif()
        add_subdirectory(myRobot)
    endif()
endif()

if (WITH_SIMULATION_MODULES AND NOT WITH_EXTERNAL_HAL)
    add_subdirectory(simulation)
endif()

configure_file(wpilib-config.cmake.in ${WPILIB_BINARY_DIR}/wpilib-config.cmake )
install(FILES ${WPILIB_BINARY_DIR}/wpilib-config.cmake DESTINATION ${wpilib_config_dir})

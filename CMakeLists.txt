# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL " ${CMAKE_CURRENT_BINARY_DIR}")
    message(
        FATAL_ERROR
        "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
"
    )
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
    set(CMAKE_SYSTEM_VERSION 10.0.18362.0 CACHE STRING INTERNAL FORCE)
    set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION 10.0.18362.0 CACHE STRING INTERNAL FORCE)
    message(STATUS "Platform version: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
endif()

# Set default build type to release with debug info (i.e. release mode optimizations
# are performed, but debug info still exists).
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
endif()

cmake_minimum_required(VERSION 3.21)
project(allwpilib)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Make timestamps of extracted files from FetchContent the time of extraction
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

set(WPILIB_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

include(CMakeDependentOption)
include(CPack)
include(OptionValidation)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${WPILIB_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${WPILIB_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${WPILIB_BINARY_DIR}/bin)

# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Options for building certain parts of the repo. Everything is built by default.
option(BUILD_SHARED_LIBS "Build with shared libs (needed for JNI)" ON)
option(WITH_JAVA "Include Java and JNI in the build" OFF)
option(WITH_JAVA_SOURCE "Build Java source jars" ${WITH_JAVA})
option(WITH_DOCS "Build Doxygen docs (needs Git for versioning)" OFF)
cmake_dependent_option(
    DOCS_WARNINGS_AS_ERRORS
    "Make docs warnings into errors"
    OFF
    WITH_DOCS
    OFF
)
option(WITH_CSCORE "Build cscore (needs OpenCV)" ON)
option(WITH_NTCORE "Build ntcore" ON)
option(WITH_WPICAL "Build wpical" OFF)
option(WITH_WPIMATH "Build wpimath" ON)
cmake_dependent_option(
    WITH_WPIUNITS
    "Build wpiunits"
    ON
    WITH_JAVA
    OFF
)
option(WITH_WPILIB "Build hal, wpilibc/j, and developerRobot (needs OpenCV)" ON)
option(WITH_EXAMPLES "Build examples" OFF)
option(WITH_TESTS "Build unit tests (requires internet connection)" ON)
option(WITH_GUI "Build GUI items" ON)
option(WITH_SIMULATION_MODULES "Build simulation modules" ON)
option(WITH_PROTOBUF "Build protobuf support" ON)

# Options for using a package manager (e.g., vcpkg) for certain dependencies.
option(USE_SYSTEM_FMTLIB "Use system fmtlib" OFF)
option(USE_SYSTEM_LIBUV "Use system libuv" OFF)
option(USE_SYSTEM_EIGEN "Use system eigen" OFF)

# Options for location of OpenCV Java.
set(OPENCV_JAVA_INSTALL_DIR "" CACHE PATH "Location to search for the OpenCV jar file")

# Options for compilation flags.
option(NO_WERROR "Disable -Werror flag during compilation" OFF)

if(NOT WITH_JAVA OR NOT WITH_CSCORE)
    if(NOT "${OPENCV_JAVA_INSTALL_DIR}" STREQUAL "")
        message(
            WARNING
            "
WARNING: OpenCV Java dir set but java is not enabled!
It will be ignored.
"
        )
    endif()
endif()

wpilib_config(OPTIONS WITH_JAVA REQUIRES BUILD_SHARED_LIBS)

wpilib_config(OPTIONS WITH_SIMULATION_MODULES REQUIRES BUILD_SHARED_LIBS WITH_WPILIB WITH_NTCORE)

wpilib_config(OPTIONS WITH_CSCORE REQUIRES WITH_NTCORE)

wpilib_config(OPTIONS WITH_GUI REQUIRES WITH_NTCORE WITH_WPIMATH)

wpilib_config(OPTIONS WITH_WPILIB REQUIRES WITH_NTCORE WITH_WPIMATH)

wpilib_config(OPTIONS WITH_WPIMATH WITH_JAVA REQUIRES WITH_WPIUNITS)

set(include_dest include)
set(java_lib_dest java)
if(WITH_JAVA OR WITH_JAVA_SOURCE)
    set(CMAKE_JAVA_COMPILE_FLAGS "-encoding" "UTF8" "-Xlint:unchecked")
    find_package(Java REQUIRED COMPONENTS Development)
    if(NOT ANDROID)
        find_package(JNI REQUIRED COMPONENTS JVM)
    endif()
endif()

if(WITH_DOCS)
    find_package(Doxygen REQUIRED)
    find_package(Git REQUIRED)
    include(AddDoxygenDocs)
    add_doxygen_docs()
endif()

if(WITH_WPICAL)
    find_package(Ceres CONFIG REQUIRED)
endif()

find_package(LIBSSH CONFIG 0.7.1)

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)
if(WITH_PROTOBUF)
    set(protobuf_MODULE_COMPATIBLE ON CACHE BOOL "" FORCE)
    find_package(Protobuf REQUIRED)
    find_program(PROTOC_COMPILER protoc REQUIRED)
endif()
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG OFF)

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(isMultiConfig)
    if(NOT "Asan" IN_LIST CMAKE_CONFIGURATION_TYPES)
        list(APPEND CMAKE_CONFIGURATION_TYPES Asan)
    endif()
    if(NOT "Tsan" IN_LIST CMAKE_CONFIGURATION_TYPES)
        list(APPEND CMAKE_CONFIGURATION_TYPES Tsan)
    endif()
    if(NOT "Ubsan" IN_LIST CMAKE_CONFIGURATION_TYPES)
        list(APPEND CMAKE_CONFIGURATION_TYPES Ubsan)
    endif()
else()
    set(allowedBuildTypes
        Asan
        Tsan
        Ubsan
        Debug
        Release
        RelWithDebInfo
        MinSizeRel
    )
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${allowedBuildTypes}")

    if(CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE IN_LIST allowedBuildTypes)
        message(FATAL_ERROR "Invalid build type: ${CMAKE_BUILD_TYPE}")
    endif()
endif()

set(CMAKE_C_FLAGS_ASAN
    "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer"
    CACHE STRING
    "Flags used by the C compiler for Asan build type or configuration."
    FORCE
)

set(CMAKE_CXX_FLAGS_ASAN
    "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer"
    CACHE STRING
    "Flags used by the C++ compiler for Asan build type or configuration."
    FORCE
)

set(CMAKE_EXE_LINKER_FLAGS_ASAN
    "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=address"
    CACHE STRING
    "Linker flags to be used to create executables for Asan build type."
    FORCE
)

set(CMAKE_SHARED_LINKER_FLAGS_ASAN
    "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=address"
    CACHE STRING
    "Linker flags to be used to create shared libraries for Asan build type."
    FORCE
)

set(CMAKE_C_FLAGS_TSAN
    "${CMAKE_C_FLAGS_DEBUG} -fsanitize=thread -fno-omit-frame-pointer"
    CACHE STRING
    "Flags used by the C compiler for Tsan build type or configuration."
    FORCE
)

set(CMAKE_CXX_FLAGS_TSAN
    "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=thread -fno-omit-frame-pointer"
    CACHE STRING
    "Flags used by the C++ compiler for Tsan build type or configuration."
    FORCE
)

set(CMAKE_EXE_LINKER_FLAGS_TSAN
    "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=thread"
    CACHE STRING
    "Linker flags to be used to create executables for Tsan build type."
    FORCE
)

set(CMAKE_SHARED_LINKER_FLAGS_TSAN
    "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=thread"
    CACHE STRING
    "Linker flags to be used to create shared libraries for Tsan build type."
    FORCE
)

set(CMAKE_C_FLAGS_UBSAN
    "${CMAKE_C_FLAGS_DEBUG} -fsanitize=undefined -fno-sanitize-recover=all -fno-omit-frame-pointer"
    CACHE STRING
    "Flags used by the C compiler for Ubsan build type or configuration."
    FORCE
)

set(CMAKE_CXX_FLAGS_UBSAN
    "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined -fno-sanitize-recover=all -fno-omit-frame-pointer"
    CACHE STRING
    "Flags used by the C++ compiler for Ubsan build type or configuration."
    FORCE
)

set(CMAKE_EXE_LINKER_FLAGS_UBSAN
    "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=undefined -fno-sanitize-recover=all"
    CACHE STRING
    "Linker flags to be used to create executables for Ubsan build type."
    FORCE
)

set(CMAKE_SHARED_LINKER_FLAGS_UBSAN
    "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fsanitize=undefined"
    CACHE STRING
    "Linker flags to be used to create shared libraries for Ubsan build type."
    FORCE
)

if(WITH_TESTS)
    enable_testing()
    add_subdirectory(thirdparty/googletest)
    include(GoogleTest)
endif()

if(USE_SYSTEM_LIBUV)
    set(LIBUV_SYSTEM_REPLACE "find_dependency(libuv CONFIG)")
endif()

if(USE_SYSTEM_EIGEN)
    set(EIGEN_SYSTEM_REPLACE "find_package(Eigen3 CONFIG)")
endif()

set(FILENAME_DEP_REPLACE "get_filename_component(SELF_DIR \"$\{CMAKE_CURRENT_LIST_FILE\}\" PATH)")
set(SELF_DIR "$\{SELF_DIR\}")
set(WPIUNITS_DEP_REPLACE_IMPL "find_dependency(wpiunits)")
set(WPIUTIL_DEP_REPLACE "find_dependency(wpiutil)")
add_subdirectory(wpiutil)

if(WITH_NTCORE)
    set(NTCORE_DEP_REPLACE "find_dependency(ntcore)")
    set(WPINET_DEP_REPLACE "find_dependency(wpinet)")
    add_subdirectory(wpinet)
    add_subdirectory(ntcore)
endif()

if(WITH_WPIMATH)
    if(WITH_JAVA)
        set(WPIUNITS_DEP_REPLACE ${WPIUNITS_DEP_REPLACE_IMPL})
        add_subdirectory(wpiunits)
    endif()
    set(WPIMATH_DEP_REPLACE "find_dependency(wpimath)")
    add_subdirectory(wpimath)
endif()

if(WITH_WPIUNITS AND NOT WITH_WPIMATH)
    # In case of building wpiunits standalone
    set(WPIUNITS_DEP_REPLACE ${WPIUNITS_DEP_REPLACE_IMPL})
    add_subdirectory(wpiunits)
endif()

if(WITH_GUI)
    add_subdirectory(fieldImages)
    add_subdirectory(thirdparty/imgui_suite)
    add_subdirectory(wpigui)
    add_subdirectory(glass)
    add_subdirectory(outlineviewer)
    add_subdirectory(sysid)
    if(WITH_WPICAL)
        add_subdirectory(wpical)
    endif()
    if(LIBSSH_FOUND)
        add_subdirectory(roborioteamnumbersetter)
        add_subdirectory(datalogtool)
    endif()
endif()

if(WITH_WPILIB OR WITH_SIMULATION_MODULES)
    set(HAL_DEP_REPLACE "find_dependency(hal)")
    add_subdirectory(hal)
endif()

if(WITH_CSCORE)
    set(CAMERASERVER_DEP_REPLACE "find_dependency(cameraserver)")
    set(CSCORE_DEP_REPLACE "find_dependency(cscore)")
    find_package(OpenCV REQUIRED)
    add_subdirectory(cscore)
    add_subdirectory(cameraserver)
endif()

if(WITH_WPILIB)
    set(APRILTAG_DEP_REPLACE "find_dependency(apriltag)")
    set(WPILIBC_DEP_REPLACE "find_dependency(wpilibc)")
    set(WPILIBJ_DEP_REPLACE "find_dependency(wpilibj)")
    set(WPILIBNEWCOMMANDS_DEP_REPLACE "find_dependency(wpilibNewCommands)")
    add_subdirectory(apriltag)
    add_subdirectory(wpilibj)
    add_subdirectory(wpilibc)
    add_subdirectory(wpilibNewCommands)
    add_subdirectory(romiVendordep)
    add_subdirectory(xrpVendordep)
    if(WITH_EXAMPLES)
        add_subdirectory(wpilibcExamples)
    endif()
    add_subdirectory(developerRobot)
endif()

if(WITH_SIMULATION_MODULES)
    add_subdirectory(simulation)
endif()

configure_file(wpilib-config.cmake.in ${WPILIB_BINARY_DIR}/wpilib-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/wpilib-config.cmake DESTINATION share/wpilib)

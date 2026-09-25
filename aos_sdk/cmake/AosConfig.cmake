# Package config for the AOS SDK.
#
# Unzip the aos-sdk headers zip, the aos-sdk static zip for the platform being
# built for, and the aos-tools zip for the platform CMake runs on, all into one
# directory. Then point CMake at its cmake/ directory and use the imported
# target:
#
#   cmake -DAos_DIR=/path/to/aos/cmake ...
#
#   find_package(Aos REQUIRED)
#   target_link_libraries(my_target PRIVATE aos::aos)
#
# See AosFunctions.cmake (included below) for aos_static_flatbuffer() and
# aos_config().

cmake_minimum_required(VERSION 3.21)

# find_package(Aos) includes this once per directory that calls it, and the
# variables below are scoped to that directory, so it has no include guard.
# What it defines globally, it defines only once.

get_filename_component(AOS_SDK_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

# Maps a CMake system name and processor to wpilib's platform directory.
function(_aos_platform_dir system processor out)
    string(TOLOWER "${processor}" processor)
    if(system STREQUAL "Linux" AND processor MATCHES "^(aarch64|arm64)$")
        set(dir "linux/systemcore")
    elseif(system STREQUAL "Linux")
        set(dir "linux/x86-64")
    elseif(system STREQUAL "Darwin")
        set(dir "osx/universal")
    elseif(system STREQUAL "Windows" AND processor MATCHES "^(aarch64|arm64)$")
        set(dir "windows/arm64")
    elseif(system STREQUAL "Windows")
        set(dir "windows/x86-64")
    else()
        message(FATAL_ERROR "The AOS SDK has no build for ${system} on ${processor}.")
    endif()
    set(${out} "${dir}" PARENT_SCOPE)
endfunction()

# The archive is for the platform being built for, and the code generators are
# for the platform CMake runs on. The two differ when cross-compiling. Either
# can be set on the command line, and then it is not detected at all, so a
# toolchain this file does not know is still usable.
#
# MSVC cross-compiles to arm64 without changing CMAKE_SYSTEM_PROCESSOR, so ask
# the compiler what it targets when it says.
if(NOT DEFINED AOS_PLATFORM)
    set(_aos_target_processor "${CMAKE_SYSTEM_PROCESSOR}")
    if(MSVC AND CMAKE_CXX_COMPILER_ARCHITECTURE_ID)
        set(_aos_target_processor "${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
    endif()
    _aos_platform_dir("${CMAKE_SYSTEM_NAME}" "${_aos_target_processor}" _aos_platform)
    set(AOS_PLATFORM "${_aos_platform}" CACHE STRING "Platform directory of the AOS archive")
endif()
if(NOT DEFINED AOS_HOST_PLATFORM)
    _aos_platform_dir("${CMAKE_HOST_SYSTEM_NAME}" "${CMAKE_HOST_SYSTEM_PROCESSOR}" _aos_host_platform)
    set(AOS_HOST_PLATFORM
        "${_aos_host_platform}"
        CACHE STRING
        "Platform directory of the AOS code generators"
    )
endif()

set(AOS_INCLUDE_DIR "${AOS_SDK_ROOT}")
set(AOS_LIBRARY_DIR "${AOS_SDK_ROOT}/${AOS_PLATFORM}/static")
set(AOS_ALWAYSLINK_DIR "${AOS_LIBRARY_DIR}/alwayslink")
set(AOS_TOOLS_DIR "${AOS_SDK_ROOT}/${AOS_HOST_PLATFORM}")
set(AOS_SHARE_DIR "${AOS_SDK_ROOT}/share/aos")
set(AOS_SCHEMA_DIR "${AOS_SHARE_DIR}/schemas")
set(AOS_FLATBUFFERS_INCLUDE_DIR "${AOS_SHARE_DIR}/flatbuffers-include")
set(AOS_BASE_CONFIG "${AOS_SHARE_DIR}/aos.json")

if(NOT EXISTS "${AOS_INCLUDE_DIR}/aos")
    message(
        FATAL_ERROR
        "The AOS headers are missing. Unzip the aos-sdk headers zip into ${AOS_SDK_ROOT}."
    )
endif()

# The archive is named the way the platform names archives: libaos.a or aos.lib,
# with a "d" suffix in a debug build. The release and debug static zips can both
# be unzipped here, and then a Debug build links the debug archives and every
# other configuration the release ones. A cross-compiling toolchain file
# confines library searches to its sysroot, hence NO_CMAKE_FIND_ROOT_PATH.
foreach(_aos_name aos aos_alwayslink)
    string(TOUPPER "${_aos_name}" _aos_var)
    find_library(
        ${_aos_var}_LIBRARY_RELEASE
        NAMES ${_aos_name}
        PATHS "${AOS_LIBRARY_DIR}" "${AOS_ALWAYSLINK_DIR}"
        NO_DEFAULT_PATH
        NO_CMAKE_FIND_ROOT_PATH
    )
    find_library(
        ${_aos_var}_LIBRARY_DEBUG
        NAMES ${_aos_name}d
        PATHS "${AOS_LIBRARY_DIR}" "${AOS_ALWAYSLINK_DIR}"
        NO_DEFAULT_PATH
        NO_CMAKE_FIND_ROOT_PATH
    )
    if(NOT ${_aos_var}_LIBRARY_RELEASE AND NOT ${_aos_var}_LIBRARY_DEBUG)
        message(
            FATAL_ERROR
            "No ${_aos_name} archive in ${AOS_LIBRARY_DIR}. Unzip the aos-sdk "
            "static zip for ${AOS_PLATFORM} into ${AOS_SDK_ROOT}."
        )
    endif()
endforeach()

# The linker options AOS's dependencies declare, generated out of the Bazel
# build for this platform. It ships in the same zip as the archive.
if(NOT EXISTS "${AOS_LIBRARY_DIR}/AosLinkOptions.cmake")
    message(
        FATAL_ERROR
        "No AosLinkOptions.cmake in ${AOS_LIBRARY_DIR}. The aos-sdk static zip "
        "unzipped there predates it; unzip a current one."
    )
endif()
include("${AOS_LIBRARY_DIR}/AosLinkOptions.cmake")

# Flatc arguments and compile definitions, generated out of the Bazel build so
# they cannot drift from it.
include("${CMAKE_CURRENT_LIST_DIR}/AosGeneratedSettings.cmake")

if(NOT EXISTS "${AOS_TOOLS_DIR}")
    message(
        FATAL_ERROR
        "No AOS code generators in ${AOS_TOOLS_DIR}. Unzip the aos-tools zip for "
        "${AOS_HOST_PLATFORM} into ${AOS_SDK_ROOT}."
    )
endif()
find_program(
    AOS_FLATC
    NAMES flatc
    PATHS "${AOS_TOOLS_DIR}"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
    REQUIRED
)
find_program(
    AOS_STATIC_FLATBUFFER_GENERATE
    NAMES generate
    PATHS "${AOS_TOOLS_DIR}"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
    REQUIRED
)
find_program(
    AOS_CONFIG_FLATTENER
    NAMES config_flattener
    PATHS "${AOS_TOOLS_DIR}"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
    REQUIRED
)

# Defines an imported archive that links the debug archive in a Debug build and
# the release one otherwise. With only one of them unzipped, every configuration
# links that one.
function(_aos_imported_archive target release debug)
    add_library(${target} STATIC IMPORTED GLOBAL)
    if(release)
        set_target_properties(${target} PROPERTIES IMPORTED_LOCATION "${release}")
    else()
        set_target_properties(${target} PROPERTIES IMPORTED_LOCATION "${debug}")
    endif()
    if(debug)
        set_property(TARGET ${target} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
        set_target_properties(${target} PROPERTIES IMPORTED_LOCATION_DEBUG "${debug}")
    endif()
endfunction()

if(NOT TARGET aos::aos)
    _aos_imported_archive(aos::archive "${AOS_LIBRARY_RELEASE}" "${AOS_LIBRARY_DEBUG}")

    # The objects Bazel marks alwayslink, which cc_static_library cannot
    # express. Linking them whole restores what a Bazel-built AOS binary does.
    # The cost is small; this archive holds only what Bazel already
    # force-links.
    _aos_imported_archive(
        aos::aos_alwayslink
        "${AOS_ALWAYSLINK_LIBRARY_RELEASE}"
        "${AOS_ALWAYSLINK_LIBRARY_DEBUG}"
    )
    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
        set(_aos_alwayslink "$<LINK_LIBRARY:WHOLE_ARCHIVE,aos::aos_alwayslink>")
    elseif(APPLE)
        set(_aos_alwayslink "-Wl,-force_load,$<TARGET_FILE:aos::aos_alwayslink>")
    elseif(MSVC)
        set(_aos_alwayslink "/WHOLEARCHIVE:$<TARGET_FILE:aos::aos_alwayslink>")
    else()
        set(_aos_alwayslink "-Wl,--whole-archive" aos::aos_alwayslink "-Wl,--no-whole-archive")
    endif()

    # An INTERFACE target rather than the imported archive itself, so the
    # whole-archive libraries can be ordered ahead of libaos.a. They reference
    # symbols that live in it, and a linker walking left to right will not go
    # back to an archive it has already passed.
    add_library(aos::aos INTERFACE IMPORTED GLOBAL)

    set_target_properties(aos::aos PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${AOS_INCLUDE_DIR}")

    # libuv is wpinet's: its objects are in libwpinet.a and its headers ship
    # with wpinet's, so aos::aos links wpinet rather than carrying a second
    # copy of either. A project that builds allwpilib alongside has the target
    # already; one on an installed wpilib gets it from wpinet's package config.
    if(NOT TARGET wpinet)
        include(CMakeFindDependencyMacro)
        find_dependency(wpinet)
    endif()

    target_link_libraries(
        aos::aos
        INTERFACE ${_aos_alwayslink} aos::archive wpinet ${AOS_LINK_OPTIONS}
    )

    # Not optional, and not only about aos/macros.h hard-erroring without
    # AOS_OS_NONE: FLATBUFFERS_MAX_ALIGNMENT and the lockless-queue defines
    # change struct layout, so a consumer compiling without them gets headers
    # that silently disagree with the archive they link against.
    target_compile_definitions(aos::aos INTERFACE ${AOS_COMPILE_DEFINITIONS})

    # The standard the archive was built with, which is also wpiutil's.
    target_compile_features(aos::aos INTERFACE cxx_std_${AOS_CXX_STANDARD})

    if(WIN32)
        # windows.h defines min and max as macros, which breaks
        # numeric_limits<>::min() in aos/time/time.h.
        target_compile_definitions(aos::aos INTERFACE NOMINMAX)
    endif()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/AosFunctions.cmake")

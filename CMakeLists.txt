cmake_minimum_required(VERSION 2.8)
project(ntcore)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNOMINMAX /D_SCL_SECURE_NO_WARNINGS /D_WINSOCK_DEPRECATED_NO_WARNINGS")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wformat=2 -Wall -Wextra -Werror -pedantic -Wno-unused-parameter")
endif()

# This must be a macro(), as inside a function string() can only
# update variables in the function scope.
macro(fix_default_compiler_settings_)
  if (MSVC)
    # For MSVC, CMake sets certain flags to defaults we want to override.
    # This replacement code is taken from sample in the CMake Wiki at
    # http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace.
    foreach (flag_var
             CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
             CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
      if (NOT BUILD_SHARED_LIBS AND NOT gtest_force_shared_crt)
        # When Google Test is built as a shared library, it should also use
        # shared runtime libraries.  Otherwise, it may end up with multiple
        # copies of runtime library data in different modules, resulting in
        # hard-to-find crashes. When it is built as a static library, it is
        # preferable to use CRT as static libraries, as we don't have to rely
        # on CRT DLLs being available. CMake always defaults to using shared
        # CRT libraries, so we override that default here.
        string(REPLACE "/MD" "-MT" ${flag_var} "${${flag_var}}")
      endif()
    endforeach()
  endif()
endmacro()
fix_default_compiler_settings_()

file(GLOB_RECURSE SRC_FILES src/*.cpp)
include_directories(include src)

# Java bindings
if (NOT WITHOUT_JAVA)
    find_package(Java)
    find_package(JNI)
    include(UseJava)
    set(CMAKE_JAVA_COMPILE_FLAGS "-Xlint:unchecked")

    include_directories(${JNI_INCLUDE_DIRS})
    list(APPEND SRC_FILES java/lib/NetworkTablesJNI.cpp)

    file(GLOB_RECURSE JAVA_SOURCES java/src/*.java)
    set(CMAKE_JNI_TARGET true)
    add_jar(ntcore_jar ${JAVA_SOURCES} OUTPUT_NAME ntcore)

    # Generate JNI headers
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/edu_wpi_first_wpilibj_networktables_NetworkTablesJNI.h"
        DEPENDS ntcore_jar java/src/edu/wpi/first/wpilibj/networktables/NetworkTablesJNI.java
        COMMAND "${Java_JAVAH_EXECUTABLE}"
        -jni
        -o "${CMAKE_CURRENT_BINARY_DIR}/edu_wpi_first_wpilibj_networktables_NetworkTablesJNI.h"
        -classpath "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ntcore_jar.dir/"
        edu.wpi.first.wpilibj.networktables.NetworkTablesJNI
        )
    include_directories("${CMAKE_CURRENT_BINARY_DIR}")
    set_source_files_properties(
        java/lib/NetworkTablesJNI.cpp
        OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/edu_wpi_first_wpilibj_networktables_NetworkTablesJNI.h"
        )

    #add_subdirectory(java/test)
endif()

if (WIN32)
	add_library(ntcore SHARED ${SRC_FILES} ntcore.def)
else()
	add_library(ntcore SHARED ${SRC_FILES})
endif()
set_target_properties(ntcore PROPERTIES VERSION 1.0.0 SOVERSION 1)
#target_link_libraries(ntcore)
if (WIN32)
	add_library(ntcore_static STATIC ${SRC_FILES})
	target_link_libraries(ntcore ws2_32)
	target_link_libraries(ntcore_static ws2_32)
endif()
INSTALL(TARGETS ntcore
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib COMPONENT lib)
INSTALL(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT headers)

# We need thread support
find_package(Threads REQUIRED)

if (NOT WITHOUT_TESTS)
    enable_testing()

    add_subdirectory(gmock)
    include_directories("gmock/include")
    include_directories("gmock/gtest/include")
    add_subdirectory(test)
endif()

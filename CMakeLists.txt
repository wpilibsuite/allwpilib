cmake_minimum_required(VERSION 2.8)
project(ntcore)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNOMINMAX /D_SCL_SECURE_NO_WARNINGS")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wformat=2 -Wall -Wextra -Werror -pedantic -Wno-unused-parameter")
endif()

file(GLOB_RECURSE SRC_FILES src/*.cpp)
include_directories(include src)
add_library(ntcore SHARED ${SRC_FILES})
set_target_properties(ntcore PROPERTIES VERSION 1.0.0 SOVERSION 1)
#target_link_libraries(ntcore)
if (WIN32)
	target_link_libraries(ntcore ws2_32)
endif()
INSTALL(TARGETS ntcore
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib COMPONENT lib)
INSTALL(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT headers)


#
#
#

# We need thread support
find_package(Threads REQUIRED)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install GoogleMock
ExternalProject_Add(
    gmock
    URL https://googlemock.googlecode.com/files/gmock-1.7.0.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gmock
    # Disable install step
    INSTALL_COMMAND ""
    CMAKE_CACHE_ARGS -DCMAKE_TOOLCHAIN_FILE:string=${CMAKE_TOOLCHAIN_FILE}
)

# Create a libgmock target to be used as a dependency by test programs
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gmock)
add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gmock)

# Set gmock properties
ExternalProject_Get_Property(gmock source_dir binary_dir)
set_target_properties(libgmock PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/libgmock.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
#    "INTERFACE_INCLUDE_DIRECTORIES" "${source_dir}/include"
)
set_target_properties(libgtest PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/gtest/libgtest.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
#    "INTERFACE_INCLUDE_DIRECTORIES" "${source_dir}/include"
)
# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/include")
include_directories("${source_dir}/gtest/include")

add_subdirectory(test)


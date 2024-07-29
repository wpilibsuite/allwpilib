project(googletest)

include(CompileWarnings)

file(
    GLOB_RECURSE googletest_src
    src/googlemock/src/*.cpp
    src/googletest/src/*.cpp
)

add_library(googletest ${googletest_src})
set_target_properties(googletest PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET googletest PROPERTY FOLDER "libraries")
target_compile_features(googletest PUBLIC cxx_std_20)

include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/googletest"
)

target_include_directories(
    googletest
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
set_target_properties(googletest
  PROPERTIES
  COMPILE_DEFINITIONS "GTEST_CREATE_SHARED_LIBRARY=1")
wpilib_target_warnings(googletest)

install(TARGETS googletest EXPORT googletest)
export(TARGETS googletest FILE googletest.cmake NAMESPACE googletest::)

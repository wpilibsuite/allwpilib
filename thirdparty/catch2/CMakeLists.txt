project(catch2)

include(CompileWarnings)

file(
    GLOB_RECURSE catch2_src
    src/main/native/cpp/*.cpp
)

add_library(catch2 ${catch2_src})
set_target_properties(catch2 PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET catch2 PROPERTY FOLDER "libraries")
target_compile_features(catch2 PUBLIC cxx_std_20)

set(CATCH_CONFIG_DEFAULT_REPORTER "console" CACHE STRING "Read docs/configuration.md for details. The name of the reporter should be without quotes.")
set(CATCH_CONFIG_CONSOLE_WIDTH "80" CACHE STRING "Read docs/configuration.md for details. Must form a valid integer literal.")

include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include"
)

configure_file(
  "src/main/native/include/catch2/catch_user_config.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/generated-includes/catch2/catch_user_config.hpp"
)

target_include_directories(
    catch2
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated-includes>
)

wpilib_target_warnings(catch2)

install(TARGETS catch2 EXPORT catch2)
export(TARGETS catch2 FILE catch2.cmake NAMESPACE Catch::)

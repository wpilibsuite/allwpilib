project(catch2)

include(CompileWarnings)

file(GLOB_RECURSE catch2_src src/main/native/cpp/*.cpp)

add_library(catch2 ${catch2_src})
set_target_properties(catch2 PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET catch2 PROPERTY FOLDER "libraries")
target_compile_features(catch2 PUBLIC cxx_std_23)

target_include_directories(
    catch2
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
)

wpilib_target_warnings(catch2)

install(TARGETS catch2 EXPORT catch2)
export(TARGETS catch2 FILE catch2.cmake NAMESPACE Catch::)

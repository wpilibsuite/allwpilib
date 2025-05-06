project(simd)

include(CompileWarnings)

file(
    GLOB_RECURSE simd_src
    src/main/native/simd/cpp/src/Simd/*.cpp
)

add_library(simd ${simd_src})
set_target_properties(simd PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET simd PROPERTY FOLDER "libraries")
target_compile_features(simd PUBLIC cxx_std_20)

include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/simd/include/src/"
)

target_include_directories(
    simd
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/simd/include/src/>
)
wpilib_target_warnings(simd)

install(TARGETS simd EXPORT simd)
export(TARGETS simd FILE simd.cmake NAMESPACE simd::)

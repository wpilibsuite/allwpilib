project(benchmark)

include(CompileWarnings)

file(GLOB benchmarkCpp_src src/main/native/cpp/*.cpp src/main/native/thirdparty/benchmark/src/*.cpp)

add_executable(benchmarkCpp ${benchmarkCpp_src})

target_compile_features(benchmarkCpp PUBLIC cxx_std_20)

wpilib_target_warnings(benchmarkCpp)

target_link_libraries(
    benchmarkCpp
    PUBLIC
        $<TARGET_NAME_IF_EXISTS:apriltag>
        $<TARGET_NAME_IF_EXISTS:wpilibc>
        $<TARGET_NAME_IF_EXISTS:wpilibNewCommands>
        $<TARGET_NAME_IF_EXISTS:wpimath>
        $<TARGET_NAME_IF_EXISTS:wpiutil>
)

# benchmark library setup
target_compile_definitions(benchmarkCpp PRIVATE benchmark_EXPORTS)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(benchmarkCpp PRIVATE shlwapi)
endif()

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(benchmarkCpp PUBLIC -DBENCHMARK_STATIC_DEFINE)
endif()

install(
    DIRECTORY src/main/native/thirdparty/benchmark/include/
    DESTINATION "${include_dest}/benchmark"
)
target_include_directories(
    benchmarkCpp
    SYSTEM
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/benchmark/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/benchmark/src>
)

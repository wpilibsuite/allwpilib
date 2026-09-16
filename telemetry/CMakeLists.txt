project(telemetry)

include(CompileWarnings)

file(GLOB telemetry_native_src src/main/native/cpp/*.cpp)

add_library(telemetry ${telemetry_native_src})
set_target_properties(telemetry PROPERTIES DEBUG_POSTFIX "d")

target_compile_features(telemetry PUBLIC cxx_std_23)
wpilib_target_warnings(telemetry)

target_include_directories(
    telemetry
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/telemetry>
)

target_link_libraries(telemetry PUBLIC wpiutil)

install(TARGETS telemetry EXPORT telemetry)
export(TARGETS telemetry FILE telemetry.cmake NAMESPACE telemetry::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/telemetry")

configure_file(telemetry-config.cmake.in ${WPILIB_BINARY_DIR}/telemetry-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/telemetry-config.cmake DESTINATION share/telemetry)
install(EXPORT telemetry DESTINATION share/telemetry)

if(WITH_TESTS)
    file(GLOB_RECURSE telemetry_testlib_src src/test/native/include/*.h)
    add_library(telemetry_testlib INTERFACE ${telemetry_test_src})
    target_include_directories(telemetry_testlib INTERFACE src/test/native/include)

    wpilib_add_test(telemetry src/test/native/cpp)
    target_link_libraries(telemetry_test telemetry telemetry_testlib wpiutil)
    if(MSVC)
        target_compile_options(telemetry_test PRIVATE /utf-8)
    endif()
endif()

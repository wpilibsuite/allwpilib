project(tunable)

include(CompileWarnings)

file(GLOB tunable_native_src src/main/native/cpp/*.cpp)

add_library(tunable ${tunable_native_src})
set_target_properties(tunable PROPERTIES DEBUG_POSTFIX "d")

target_compile_features(tunable PUBLIC cxx_std_23)
wpilib_target_warnings(tunable)

target_include_directories(
    tunable
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/tunable>
)

target_link_libraries(tunable PUBLIC wpiutil)

install(TARGETS tunable EXPORT tunable)
export(TARGETS tunable FILE tunable.cmake NAMESPACE tunable::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/tunable")

configure_file(tunable-config.cmake.in ${WPILIB_BINARY_DIR}/tunable-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/tunable-config.cmake DESTINATION share/tunable)
install(EXPORT tunable DESTINATION share/tunable)

if(WITH_TESTS)
    file(GLOB_RECURSE tunable_testlib_src src/test/native/include/*.h)
    add_library(tunable_testlib INTERFACE ${tunable_test_src})
    target_include_directories(tunable_testlib INTERFACE src/test/native/include)

    wpilib_add_test(tunable src/test/native/cpp)
    target_link_libraries(tunable_test tunable tunable_testlib wpiutil)
    if(MSVC)
        target_compile_options(tunable_test PRIVATE /utf-8)
    endif()
endif()

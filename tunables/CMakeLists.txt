project(tunables)

include(CompileWarnings)

file(GLOB tunables_native_src src/main/native/cpp/*.cpp)

add_library(tunables ${tunables_native_src})
set_target_properties(tunables PROPERTIES DEBUG_POSTFIX "d")

target_compile_features(tunables PUBLIC cxx_std_23)
wpilib_target_warnings(tunables)

target_include_directories(
    tunables
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/tunables>
)

target_link_libraries(tunables PUBLIC wpiutil)

install(TARGETS tunables EXPORT tunables)
export(TARGETS tunables FILE tunables.cmake NAMESPACE tunables::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/tunables")

configure_file(tunables-config.cmake.in ${WPILIB_BINARY_DIR}/tunables-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/tunables-config.cmake DESTINATION share/tunables)
install(EXPORT tunables DESTINATION share/tunables)

if(WITH_TESTS)
    file(GLOB_RECURSE tunables_testlib_src src/test/native/include/*.h)
    add_library(tunables_testlib INTERFACE ${tunables_testlib_src})
    target_include_directories(tunables_testlib INTERFACE src/test/native/include)

    wpilib_add_test(tunables src/test/native/cpp)
    target_link_libraries(tunables_test tunables tunables_testlib wpiutil)
    if(MSVC)
        target_compile_options(tunables_test PRIVATE /utf-8)
    endif()
endif()

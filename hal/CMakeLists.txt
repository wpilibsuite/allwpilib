project(hal)

include(CompileWarnings)
include(AddTest)

file(
    GLOB hal_shared_native_src
    src/main/native/cpp/*.cpp
    src/main/native/cpp/cpp/*.cpp
    src/main/native/cpp/handles/*.cpp
)
file(GLOB_RECURSE hal_sim_native_src src/main/native/sim/*.cpp)
add_library(hal ${hal_shared_native_src} ${hal_sim_native_src})
wpilib_target_warnings(hal)
set_target_properties(hal PROPERTIES DEBUG_POSTFIX "d")
set_target_properties(hal PROPERTIES OUTPUT_NAME "wpiHal")

target_include_directories(
    hal
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/hal>
)
target_link_libraries(hal PUBLIC ntcore wpiutil)

set_property(TARGET hal PROPERTY FOLDER "libraries")

install(TARGETS hal EXPORT hal)
export(TARGETS hal FILE hal.cmake NAMESPACE hal::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/hal")

configure_file(hal-config.cmake.in ${WPILIB_BINARY_DIR}/hal-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/hal-config.cmake DESTINATION share/hal)
install(EXPORT hal DESTINATION share/hal)

if(WPILIB_WITH_TESTS)
    wpilib_add_test_catch2(hal src/test/native/cpp)
    target_link_libraries(hal_test hal)
endif()

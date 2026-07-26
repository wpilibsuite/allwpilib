project(drivers)

include(AddTest)
include(CompileWarnings)

file(
    GLOB_RECURSE drivers_native_src
    src/main/native/cpp/*.cpp
    src/generated/main/native/cpp/*.cpp
)
add_library(drivers ${drivers_native_src})
set_target_properties(drivers PROPERTIES DEBUG_POSTFIX "d")
set_property(TARGET drivers PROPERTY FOLDER "libraries")

target_compile_features(drivers PUBLIC cxx_std_23)
wpilib_target_warnings(drivers)
target_link_libraries(drivers wpilibc)

target_include_directories(
    drivers
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/drivers>
)

install(TARGETS drivers EXPORT drivers)
export(TARGETS drivers FILE drivers.cmake NAMESPACE drivers::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/drivers")
install(DIRECTORY src/generated/main/native/include/ DESTINATION "${include_dest}/drivers")

configure_file(drivers-config.cmake.in ${WPILIB_BINARY_DIR}/drivers-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/drivers-config.cmake DESTINATION share/drivers)
install(EXPORT drivers DESTINATION share/drivers)

if(WPILIB_WITH_TESTS)
    wpilib_add_test(drivers src/test/native/cpp)
    target_link_libraries(drivers_test drivers googletest)
endif()

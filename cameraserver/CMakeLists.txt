project(cameraserver)

include(CompileWarnings)
include(AddTest)

file(GLOB_RECURSE cameraserver_native_src src/main/native/cpp/*.cpp)
add_library(cameraserver ${cameraserver_native_src})
set_target_properties(cameraserver PROPERTIES DEBUG_POSTFIX "d")
target_include_directories(
    cameraserver
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/cameraserver>
)
wpilib_target_warnings(cameraserver)
target_link_libraries(cameraserver PUBLIC ntcore cscore wpiutil)

set_property(TARGET cameraserver PROPERTY FOLDER "libraries")

install(TARGETS cameraserver EXPORT cameraserver)
export(TARGETS cameraserver FILE cameraserver.cmake NAMESPACE cameraserver::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/cameraserver")

configure_file(cameraserver-config.cmake.in ${WPILIB_BINARY_DIR}/cameraserver-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/cameraserver-config.cmake DESTINATION share/cameraserver)
install(EXPORT cameraserver DESTINATION share/cameraserver)

file(GLOB multiCameraServer_src multiCameraServer/src/main/native/cpp/*.cpp)
add_executable(multiCameraServer ${multiCameraServer_src})
wpilib_target_warnings(multiCameraServer)
target_link_libraries(multiCameraServer cameraserver)

set_property(TARGET multiCameraServer PROPERTY FOLDER "examples")

if(WPILIB_WITH_TESTS)
    wpilib_add_test(cameraserver src/test/native/cpp)
    target_link_libraries(cameraserver_test cameraserver googletest)
endif()

project(wpilibcpp)

include(CompileWarnings)
include(AddTest)

configure_file(src/generate/WPILibVersion.cpp.in WPILibVersion.cpp)

file(GLOB_RECURSE
    wpilibcpp_native_src src/main/native/cpp/*.cpp src/main/native/cppcs/*.cpp)

add_library(wpilibcpp ${wpilibcpp_native_src} ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.cpp)
set_target_properties(wpilibcpp PROPERTIES DEBUG_POSTFIX "d")

target_include_directories(wpilibcpp PUBLIC
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
                            $<INSTALL_INTERFACE:${include_dest}/wpilibcpp>)
wpilib_target_warnings(wpilibcpp)

if (WITH_CSCORE)
    find_package( OpenCV )
    target_link_libraries(wpilibcpp PUBLIC cameraserver cscore ${OpenCV_LIBS})
else()
    target_compile_definitions(wpilibcpp PRIVATE DYNAMIC_CAMERA_SERVER)
    # Add just the camera server include directory
    target_include_directories(wpilibcpp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../cameraserver/src/main/native/include)
endif()

target_link_libraries(wpilibcpp PUBLIC hal ntcore wpimath wpiutil)


set_property(TARGET wpilibcpp PROPERTY FOLDER "libraries")

install(TARGETS wpilibcpp EXPORT wpilibcpp DESTINATION "${main_lib_dest}")
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/wpilibcpp")

if (WITH_FLAT_INSTALL)
    set (wpilibcpp_config_dir ${wpilib_dest})
else()
    set (wpilibcpp_config_dir share/wpilibcpp)
endif()

configure_file(wpilibcpp-config.cmake.in ${WPILIB_BINARY_DIR}/wpilibcpp-config.cmake )
install(FILES ${WPILIB_BINARY_DIR}/wpilibcpp-config.cmake DESTINATION ${wpilibcpp_config_dir})
install(EXPORT wpilibcpp DESTINATION ${wpilibcpp_config_dir})

if (WITH_TESTS)
    wpilib_add_test(wpilibcpp src/test/native/cpp)
    target_include_directories(wpilibcpp_test PRIVATE src/test/native/include)
    target_link_libraries(wpilibcpp_test wpilibcpp gmock_main)
    if (NOT WITH_CSCORE)
        target_compile_definitions(wpilibcpp_test PRIVATE DYNAMIC_CAMERA_SERVER)
        # Add just the camera server include directory
        target_include_directories(wpilibcpp_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../cameraserver/src/main/native/include)
    endif()
endif()

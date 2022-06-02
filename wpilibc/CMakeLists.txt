project(wpilibc)

include(CompileWarnings)
include(AddTest)

configure_file(src/generate/WPILibVersion.cpp.in WPILibVersion.cpp)

file(GLOB_RECURSE
    wpilibc_native_src src/main/native/cpp/*.cpp src/main/native/cppcs/*.cpp)

add_library(wpilibc ${wpilibc_native_src} ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.cpp)
set_target_properties(wpilibc PROPERTIES DEBUG_POSTFIX "d")

target_include_directories(wpilibc PUBLIC
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
                            $<INSTALL_INTERFACE:${include_dest}/wpilibc>)
wpilib_target_warnings(wpilibc)

if (WITH_CSCORE)
    find_package( OpenCV )
    target_link_libraries(wpilibc PUBLIC cameraserver cscore ${OpenCV_LIBS})
else()
    target_compile_definitions(wpilibc PRIVATE DYNAMIC_CAMERA_SERVER)
    # Add just the camera server include directory
    target_include_directories(wpilibc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../cameraserver/src/main/native/include)
endif()

target_link_libraries(wpilibc PUBLIC hal ntcore wpimath wpiutil)


set_property(TARGET wpilibc PROPERTY FOLDER "libraries")

install(TARGETS wpilibc EXPORT wpilibc DESTINATION "${main_lib_dest}")
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/wpilibc")

if (WITH_FLAT_INSTALL)
    set (wpilibc_config_dir ${wpilib_dest})
else()
    set (wpilibc_config_dir share/wpilibc)
endif()

configure_file(wpilibc-config.cmake.in ${WPILIB_BINARY_DIR}/wpilibc-config.cmake )
install(FILES ${WPILIB_BINARY_DIR}/wpilibc-config.cmake DESTINATION ${wpilibc_config_dir})
install(EXPORT wpilibc DESTINATION ${wpilibc_config_dir})

if (WITH_TESTS)
    wpilib_add_test(wpilibc src/test/native/cpp)
    target_include_directories(wpilibc_test PRIVATE src/test/native/include)
    target_link_libraries(wpilibc_test wpilibc gmock_main)
    if (NOT WITH_CSCORE)
        target_compile_definitions(wpilibc_test PRIVATE DYNAMIC_CAMERA_SERVER)
        # Add just the camera server include directory
        target_include_directories(wpilibc_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../cameraserver/src/main/native/include)
    endif()
endif()

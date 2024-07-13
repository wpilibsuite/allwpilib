project(wpilibc)

include(CompileWarnings)
include(AddTest)

configure_file(src/generate/WPILibVersion.cpp.in WPILibVersion.cpp)

file(
    GLOB_RECURSE wpilibc_native_src
    src/main/native/cpp/*.cpp
    src/main/native/cppcs/*.cpp
    src/generated/main/native/cpp/*.cpp
)

add_library(wpilibc ${wpilibc_native_src} ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.cpp)
set_target_properties(wpilibc PROPERTIES DEBUG_POSTFIX "d")

target_include_directories(
    wpilibc
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/wpilibc>
)
wpilib_target_warnings(wpilibc)

if(WITH_CSCORE)
    target_link_libraries(wpilibc PUBLIC cameraserver cscore ${OpenCV_LIBS})
else()
    target_compile_definitions(wpilibc PRIVATE DYNAMIC_CAMERA_SERVER)
    # Add just the camera server include directory
    target_include_directories(
        wpilibc
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../cameraserver/src/main/native/include
    )
endif()

target_link_libraries(wpilibc PUBLIC hal ntcore wpimath wpiutil)

set_property(TARGET wpilibc PROPERTY FOLDER "libraries")

install(TARGETS wpilibc EXPORT wpilibc)
export(TARGETS wpilibc FILE wpilibc.cmake NAMESPACE wpilibc::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/wpilibc")
install(DIRECTORY src/generated/main/native/include/ DESTINATION "${include_dest}/wpilibc")

configure_file(wpilibc-config.cmake.in ${WPILIB_BINARY_DIR}/wpilibc-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/wpilibc-config.cmake DESTINATION share/wpilibc)
install(EXPORT wpilibc DESTINATION share/wpilibc)

if(WITH_TESTS)
    wpilib_add_test(wpilibc src/test/native/cpp)
    target_include_directories(wpilibc_test PRIVATE src/test/native/include)
    target_link_libraries(wpilibc_test wpilibc googletest)
    if(NOT WITH_CSCORE)
        target_compile_definitions(wpilibc_test PRIVATE DYNAMIC_CAMERA_SERVER)
        # Add just the camera server include directory
        target_include_directories(
            wpilibc_test
            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../cameraserver/src/main/native/include
        )
    endif()
endif()

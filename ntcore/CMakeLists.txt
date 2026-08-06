project(ntcore)

include(CompileWarnings)
include(AddTest)

file(
    GLOB ntcore_native_src
    src/main/native/cpp/*.cpp
    src/generated/main/native/cpp/*.cpp
    src/main/native/cpp/local/*.cpp
    src/main/native/cpp/net/*.cpp
    src/main/native/cpp/net3/*.cpp
    src/main/native/cpp/networktables/*.cpp
    src/main/native/cpp/server/*.cpp
    src/main/native/cpp/tables/*.cpp
)
add_library(ntcore ${ntcore_native_src})
set_target_properties(ntcore PROPERTIES DEBUG_POSTFIX "d")
target_include_directories(
    ntcore
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/cpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/ntcore>
)
wpilib_target_warnings(ntcore)
target_compile_features(ntcore PUBLIC cxx_std_23)
target_link_libraries(ntcore PUBLIC wpinet wpiutil datalog)

set_property(TARGET ntcore PROPERTY FOLDER "libraries")

install(TARGETS ntcore EXPORT ntcore)
export(TARGETS ntcore FILE ntcore.cmake NAMESPACE ntcore::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/ntcore")
install(DIRECTORY src/generated/main/native/include/ DESTINATION "${include_dest}/ntcore")

configure_file(ntcore-config.cmake.in ${WPILIB_BINARY_DIR}/ntcore-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/ntcore-config.cmake DESTINATION share/ntcore)
install(EXPORT ntcore DESTINATION share/ntcore)

if(WPILIB_WITH_EXECUTABLES)
    add_executable(ntcoredev src/dev/native/cpp/main.cpp)
    wpilib_target_warnings(ntcoredev)
    target_link_libraries(ntcoredev ntcore)
endif()

if(WPILIB_WITH_TESTS)
    wpilib_add_test_catch2(ntcore src/test/native/cpp)
    target_include_directories(ntcore_test PRIVATE src/main/native/cpp)
    target_link_libraries(ntcore_test ntcore wpiutil_testlib)
endif()

project(xrpVendordep)

include(SubDirList)
include(CompileWarnings)
include(AddTest)

file(GLOB_RECURSE xrpVendordep_native_src src/main/native/cpp/*.cpp)
add_library(xrpVendordep ${xrpVendordep_native_src})
set_target_properties(xrpVendordep PROPERTIES DEBUG_POSTFIX "d")
set_property(TARGET xrpVendordep PROPERTY FOLDER "libraries")

target_compile_features(xrpVendordep PUBLIC cxx_std_23)
wpilib_target_warnings(xrpVendordep)
target_link_libraries(xrpVendordep wpilibc)

target_include_directories(
    xrpVendordep
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/xrpVendordep>
)

install(TARGETS xrpVendordep EXPORT xrpvendordep)
export(TARGETS xrpVendordep FILE xrpvendordep.cmake NAMESPACE xrpvendordep::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/xrpVendordep")

configure_file(xrpvendordep-config.cmake.in ${WPILIB_BINARY_DIR}/xrpvendordep-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/xrpvendordep-config.cmake DESTINATION share/xrpVendordep)
install(EXPORT xrpvendordep DESTINATION share/xrpVendordep)

if(WITH_TESTS)
    wpilib_add_test_catch2(xrpVendordep src/test/native/cpp)
    target_include_directories(xrpVendordep_test PRIVATE src/test/native/include)
    target_link_libraries(xrpVendordep_test xrpVendordep)
endif()

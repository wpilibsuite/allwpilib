project(romiVendordep)

include(SubDirList)
include(CompileWarnings)
include(AddTest)

file(GLOB_RECURSE romiVendordep_native_src src/main/native/cpp/*.cpp)
add_library(romiVendordep ${romiVendordep_native_src})
set_target_properties(romiVendordep PROPERTIES DEBUG_POSTFIX "d")
set_property(TARGET romiVendordep PROPERTY FOLDER "libraries")

target_compile_features(romiVendordep PUBLIC cxx_std_23)
wpilib_target_warnings(romiVendordep)
target_link_libraries(romiVendordep wpilibc)

target_include_directories(
    romiVendordep
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/romiVendordep>
)

install(TARGETS romiVendordep EXPORT romivendordep)
export(TARGETS romiVendordep FILE romivendordep.cmake NAMESPACE romivendordep::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/romiVendordep")

configure_file(romivendordep-config.cmake.in ${WPILIB_BINARY_DIR}/romivendordep-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/romivendordep-config.cmake DESTINATION share/romiVendordep)
install(EXPORT romivendordep DESTINATION share/romiVendordep)

if(WITH_TESTS)
    wpilib_add_test(romiVendordep src/test/native/cpp)
    target_include_directories(romiVendordep_test PRIVATE src/test/native/include)
    target_link_libraries(romiVendordep_test romiVendordep googletest)
endif()

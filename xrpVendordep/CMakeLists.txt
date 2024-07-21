project(xrpVendordep)

include(SubDirList)
include(CompileWarnings)
include(AddTest)

if(WITH_JAVA)
    include(UseJava)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
    add_jar(
        xrpVendordep_jar
        ${JAVA_SOURCES}
        INCLUDE_JARS
            hal_jar
            ntcore_jar
            cscore_jar
            cameraserver_jar
            wpimath_jar
            wpiunits_jar
            wpiutil_jar
            wpilibj_jar
        OUTPUT_NAME xrpVendordep
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET xrpVendordep_jar PROPERTY FOLDER "java")

    install_jar(xrpVendordep_jar DESTINATION ${java_lib_dest})
    install_jar_exports(
        TARGETS xrpVendordep_jar
        FILE xrpVendordep_jar.cmake
        DESTINATION share/xrpVendordep
    )
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    add_source_jar(
        xrpVendordep_src_jar
        BASE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
        OUTPUT_NAME xrpVendordep-sources
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET xrpVendordep_src_jar PROPERTY FOLDER "java")

    install_jar(xrpVendordep_src_jar DESTINATION ${java_lib_dest})
endif()

file(GLOB_RECURSE xrpVendordep_native_src src/main/native/cpp/*.cpp)
add_library(xrpVendordep ${xrpVendordep_native_src})
set_target_properties(xrpVendordep PROPERTIES DEBUG_POSTFIX "d")
set_property(TARGET xrpVendordep PROPERTY FOLDER "libraries")

target_compile_features(xrpVendordep PUBLIC cxx_std_20)
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
    wpilib_add_test(xrpVendordep src/test/native/cpp)
    target_include_directories(xrpVendordep_test PRIVATE src/test/native/include)
    target_link_libraries(xrpVendordep_test xrpVendordep googletest)
endif()

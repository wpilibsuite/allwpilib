project(xrpVendordep)

include(SubDirList)
include(CompileWarnings)
include(AddTest)

if(WITH_JAVA)
    find_package(Java REQUIRED)
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
    find_package(Java REQUIRED)
    include(UseJava)
    file(GLOB XRPVENDORDEP_SOURCES src/main/java/edu/wpi/first/wpilibj/xrp/*.java)
    add_jar(
        xrpVendordep_src_jar
        RESOURCES
        NAMESPACE "edu/wpi/first/wpilibj/xrp" ${XRPVENDORDEP_SOURCES}
        OUTPUT_NAME xrpVendordep-sources
    )

    get_property(xrpVendordep_src_JAR_FILE TARGET xrpVendordep_src_jar PROPERTY JAR_FILE)
    install(FILES ${xrpVendordep_src_JAR_FILE} DESTINATION "${java_lib_dest}")

    set_property(TARGET xrpVendordep_src_jar PROPERTY FOLDER "java")
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

install(TARGETS xrpVendordep EXPORT xrpvendordep DESTINATION "${main_lib_dest}")
export(TARGETS xrpVendordep FILE xrpvendordep.cmake NAMESPACE xrpvendordep::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/xrpVendordep")

configure_file(xrpvendordep-config.cmake.in ${WPILIB_BINARY_DIR}/xrpvendordep-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/xrpvendordep-config.cmake DESTINATION share/xrpVendordep)
install(EXPORT xrpvendordep DESTINATION share/xrpVendordep)

if(WITH_TESTS)
    wpilib_add_test(xrpVendordep src/test/native/cpp)
    target_include_directories(xrpVendordep_test PRIVATE src/test/native/include)
    target_link_libraries(xrpVendordep_test xrpVendordep gmock_main)
endif()

project(telemetry)

include(CompileWarnings)

file(GLOB telemetry_native_src src/main/native/cpp/*.cpp)

add_library(telemetry ${telemetry_native_src})
set_target_properties(telemetry PROPERTIES DEBUG_POSTFIX "d")

target_compile_features(telemetry PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(
        telemetry
        PUBLIC /permissive- /Zc:preprocessor /Zc:__cplusplus /Zc:throwingNew /MP /bigobj /utf-8
    )
    target_compile_definitions(telemetry PRIVATE -D_CRT_SECURE_NO_WARNINGS)
endif()
wpilib_target_warnings(telemetry)

target_include_directories(
    telemetry
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
)

target_link_libraries(telemetry PUBLIC wpiutil)

# Java bindings
if(WITH_JAVA)
    include(UseJava)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
    file(GLOB QUICKBUF_JAR ${WPILIB_BINARY_DIR}/wpiutil/thirdparty/quickbuf/*.jar)

    add_jar(
        telemetry_jar
        ${JAVA_SOURCES}
        # INCLUDE_JARS ${JACKSON_JARS} ${QUICKBUF_JAR}
        INCLUDE_JARS wpiutil_jar ${QUICKBUF_JAR}
        OUTPUT_NAME telemetry
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET telemetry_jar PROPERTY FOLDER "java")

    install_jar(telemetry_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS telemetry_jar FILE telemetry_jar.cmake DESTINATION share/telemetry)
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    add_source_jar(
        telemetry_src_jar
        BASE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
        OUTPUT_NAME telemetry-sources
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET telemetry_src_jar PROPERTY FOLDER "java")

    install_jar(telemetry_src_jar DESTINATION ${java_lib_dest})
endif()

install(TARGETS telemetry EXPORT telemetry)
export(TARGETS telemetry FILE telemetry.cmake NAMESPACE telemetry::)

configure_file(telemetry-config.cmake.in ${WPILIB_BINARY_DIR}/telemetry-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/telemetry-config.cmake DESTINATION share/telemetry)
install(EXPORT telemetry DESTINATION share/telemetry)

if(WITH_TESTS)
    file(GLOB_RECURSE telemetry_testlib_src src/test/native/include/*.h)
    add_library(telemetry_testlib INTERFACE ${telemetry_test_src})
    target_include_directories(telemetry_testlib INTERFACE src/test/native/include)

    wpilib_add_test(telemetry src/test/native/cpp)
    target_link_libraries(telemetry_test telemetry googletest telemetry_testlib wpiutil)
    if(MSVC)
        target_compile_options(telemetry_test PRIVATE /utf-8)
    endif()
endif()

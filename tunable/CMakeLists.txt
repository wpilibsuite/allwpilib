project(tunable)

include(CompileWarnings)

file(GLOB tunable_native_src src/main/native/cpp/*.cpp)

add_library(tunable ${tunable_native_src})
set_target_properties(tunable PROPERTIES DEBUG_POSTFIX "d")

target_compile_features(tunable PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(
        tunable
        PUBLIC /permissive- /Zc:preprocessor /Zc:__cplusplus /Zc:throwingNew /MP /bigobj /utf-8
    )
    target_compile_definitions(tunable PRIVATE -D_CRT_SECURE_NO_WARNINGS)
endif()
wpilib_target_warnings(tunable)

target_include_directories(
    tunable
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
)

target_link_libraries(tunable PUBLIC wpiutil)

# Java bindings
if(WITH_JAVA)
    include(UseJava)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
    file(GLOB QUICKBUF_JAR ${WPILIB_BINARY_DIR}/wpiutil/thirdparty/quickbuf/*.jar)

    add_jar(
        tunable_jar
        ${JAVA_SOURCES}
        # INCLUDE_JARS ${JACKSON_JARS} ${QUICKBUF_JAR}
        INCLUDE_JARS wpiutil_jar ${QUICKBUF_JAR}
        OUTPUT_NAME tunable
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET tunable_jar PROPERTY FOLDER "java")

    install_jar(tunable_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS tunable_jar FILE tunable_jar.cmake DESTINATION share/tunable)
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    add_source_jar(
        tunable_src_jar
        BASE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
        OUTPUT_NAME tunable-sources
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET tunable_src_jar PROPERTY FOLDER "java")

    install_jar(tunable_src_jar DESTINATION ${java_lib_dest})
endif()

install(TARGETS tunable EXPORT tunable)
export(TARGETS tunable FILE tunable.cmake NAMESPACE tunable::)

configure_file(tunable-config.cmake.in ${WPILIB_BINARY_DIR}/tunable-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/tunable-config.cmake DESTINATION share/tunable)
install(EXPORT tunable DESTINATION share/tunable)

if(WITH_TESTS)
    file(GLOB_RECURSE tunable_testlib_src src/test/native/include/*.h)
    add_library(tunable_testlib INTERFACE ${tunable_test_src})
    target_include_directories(tunable_testlib INTERFACE src/test/native/include)

    wpilib_add_test(tunable src/test/native/cpp)
    target_link_libraries(tunable_test tunable googletest tunable_testlib wpiutil)
    if(MSVC)
        target_compile_options(tunable_test PRIVATE /utf-8)
    endif()
endif()

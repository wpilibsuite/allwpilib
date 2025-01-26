project(libdatalog)

include(CompileWarnings)

file(GLOB libdatalog_native_src src/main/native/cpp/*.cpp)

file(GLOB libdatalog_jni_src src/main/native/cpp/jni/DataLogJNI.cpp)
list(REMOVE_ITEM libdatalog_native_src ${libdatalog_jni_src})

add_library(libdatalog ${libdatalog_native_src})
set_target_properties(libdatalog PROPERTIES DEBUG_POSTFIX "d")

target_compile_features(libdatalog PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(
        libdatalog
        PUBLIC /permissive- /Zc:preprocessor /Zc:__cplusplus /Zc:throwingNew /MP /bigobj /utf-8
    )
    target_compile_definitions(libdatalog PRIVATE -D_CRT_SECURE_NO_WARNINGS)
endif()
wpilib_target_warnings(libdatalog)

target_include_directories(
    libdatalog
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
)

target_link_libraries(libdatalog PRIVATE wpiutil)

subdir_list(libdatalog_examples "${CMAKE_CURRENT_SOURCE_DIR}/examples")
foreach(example ${libdatalog_examples})
    file(GLOB libdatalog_example_src examples/${example}/*.cpp)
    if(libdatalog_example_src)
        add_executable(libdatalog_${example} ${libdatalog_example_src})
        wpilib_target_warnings(libdatalog_${example})
        target_link_libraries(libdatalog_${example} libdatalog wpiutil)
        set_property(TARGET libdatalog_${example} PROPERTY FOLDER "examples")
    endif()
endforeach()

# Java bindings
if(WITH_JAVA)
    include(UseJava)

    set(CMAKE_JNI_TARGET true)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
    file(GLOB QUICKBUF_JAR ${WPILIB_BINARY_DIR}/wpiutil/thirdparty/quickbuf/*.jar)

    add_jar(
        libdatalog_jar
        ${JAVA_SOURCES}
        # INCLUDE_JARS ${JACKSON_JARS} ${QUICKBUF_JAR}
        INCLUDE_JARS wpiutil_jar ${QUICKBUF_JAR}
        OUTPUT_NAME libdatalog
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
        GENERATE_NATIVE_HEADERS libdatalog_jni_headers
    )
    set_property(TARGET libdatalog_jar PROPERTY FOLDER "java")

    install_jar(libdatalog_jar DESTINATION ${java_lib_dest})
    install_jar_exports(
        TARGETS libdatalog_jar
        FILE libdatalog_jar.cmake
        DESTINATION share/libdatalog
    )

    add_library(libdatalogjni ${libdatalog_jni_src})
    wpilib_target_warnings(libdatalogjni)
    target_link_libraries(libdatalogjni PUBLIC libdatalog wpiutil)

    set_property(TARGET libdatalogjni PROPERTY FOLDER "libraries")

    target_link_libraries(libdatalogjni PRIVATE libdatalog_jni_headers)
    add_dependencies(libdatalogjni libdatalog_jar)

    install(TARGETS libdatalogjni EXPORT libdatalogjni)
    export(TARGETS libdatalogjni FILE libdatalogjni.cmake NAMESPACE libdatalogjni::)
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    add_source_jar(
        libdatalog_src_jar
        BASE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
        OUTPUT_NAME libdatalog-sources
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET libdatalog_src_jar PROPERTY FOLDER "java")

    install_jar(libdatalog_src_jar DESTINATION ${java_lib_dest})
endif()

install(TARGETS libdatalog EXPORT libdatalog)
export(TARGETS libdatalog FILE libdatalog.cmake NAMESPACE libdatalog::)

configure_file(libdatalog-config.cmake.in ${WPILIB_BINARY_DIR}/libdatalog-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/libdatalog-config.cmake DESTINATION share/libdatalog)
install(EXPORT libdatalog DESTINATION share/libdatalog)

if(WITH_TESTS)
    file(GLOB_RECURSE libdatalog_testlib_src src/test/native/include/*.h)
    add_library(libdatalog_testlib INTERFACE ${libdatalog_test_src})
    target_include_directories(libdatalog_testlib INTERFACE src/test/native/include)

    wpilib_add_test(libdatalog src/test/native/cpp)
    target_link_libraries(libdatalog_test libdatalog googletest libdatalog_testlib wpiutil)
    if(MSVC)
        target_compile_options(libdatalog_test PRIVATE /utf-8)
    endif()
endif()

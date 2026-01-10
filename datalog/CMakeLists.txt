project(datalog)

include(CompileWarnings)

file(GLOB datalog_native_src src/main/native/cpp/*.cpp)

file(GLOB datalog_jni_src src/main/native/cpp/jni/DataLogJNI.cpp)
list(REMOVE_ITEM datalog_native_src ${datalog_jni_src})

add_library(datalog ${datalog_native_src})
set_target_properties(datalog PROPERTIES DEBUG_POSTFIX "d")

target_compile_features(datalog PUBLIC cxx_std_20)
wpilib_target_warnings(datalog)

target_include_directories(
    datalog
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
)

target_link_libraries(datalog PUBLIC telemetry wpiutil)

subdir_list(datalog_examples "${CMAKE_CURRENT_SOURCE_DIR}/examples")
foreach(example ${datalog_examples})
    file(GLOB datalog_example_src examples/${example}/*.cpp)
    if(datalog_example_src)
        add_executable(datalog_${example} ${datalog_example_src})
        wpilib_target_warnings(datalog_${example})
        target_link_libraries(datalog_${example} datalog wpiutil)
        set_property(TARGET datalog_${example} PROPERTY FOLDER "examples")
    endif()
endforeach()

# Java bindings
if(WITH_JAVA)
    include(UseJava)

    set(CMAKE_JNI_TARGET true)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
    file(GLOB QUICKBUF_JAR ${WPILIB_BINARY_DIR}/wpiutil/thirdparty/quickbuf/*.jar)

    add_jar(
        datalog_jar
        ${JAVA_SOURCES}
        INCLUDE_JARS telemetry_jar wpiutil_jar ${QUICKBUF_JAR}
        OUTPUT_NAME datalog
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
        GENERATE_NATIVE_HEADERS datalog_jni_headers
    )
    set_property(TARGET datalog_jar PROPERTY FOLDER "java")

    install_jar(datalog_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS datalog_jar FILE datalog_jar.cmake DESTINATION share/datalog)

    add_library(datalogjni ${datalog_jni_src})
    wpilib_target_warnings(datalogjni)
    target_link_libraries(datalogjni PUBLIC datalog wpiutil)

    set_property(TARGET datalogjni PROPERTY FOLDER "libraries")

    target_link_libraries(datalogjni PRIVATE datalog_jni_headers)
    add_dependencies(datalogjni datalog_jar)

    install(TARGETS datalogjni EXPORT datalogjni)
    export(TARGETS datalogjni FILE datalogjni.cmake NAMESPACE datalogjni::)
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    add_source_jar(
        datalog_src_jar
        BASE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
        OUTPUT_NAME datalog-sources
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET datalog_src_jar PROPERTY FOLDER "java")

    install_jar(datalog_src_jar DESTINATION ${java_lib_dest})
endif()

install(TARGETS datalog EXPORT datalog)
export(TARGETS datalog FILE datalog.cmake NAMESPACE datalog::)

configure_file(datalog-config.cmake.in ${WPILIB_BINARY_DIR}/datalog-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/datalog-config.cmake DESTINATION share/datalog)
install(EXPORT datalog DESTINATION share/datalog)

if(WITH_TESTS)
    file(GLOB_RECURSE datalog_testlib_src src/test/native/include/*.h)
    add_library(datalog_testlib INTERFACE ${datalog_test_src})
    target_include_directories(datalog_testlib INTERFACE src/test/native/include)

    wpilib_add_test(datalog src/test/native/cpp)
    target_link_libraries(datalog_test datalog googletest datalog_testlib wpiutil)
    if(MSVC)
        target_compile_options(datalog_test PRIVATE /utf-8)
    endif()
endif()

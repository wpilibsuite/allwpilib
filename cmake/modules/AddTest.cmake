include(CompileWarnings)

macro(wpilib_add_test name srcdir)
    file(GLOB_RECURSE test_src ${srcdir}/*.cpp)
    if(NOT WITH_PROTOBUF)
        list(FILTER test_src EXCLUDE REGEX "/proto/")
    endif()
    add_executable(${name}_test ${test_src})
    set_property(TARGET ${name}_test PROPERTY FOLDER "tests")
    wpilib_target_warnings(${name}_test)
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(${name}_test PRIVATE -DGTEST_LINKED_AS_SHARED_LIBRARY)
    endif()
    if(MSVC)
        target_compile_options(${name}_test PRIVATE /wd4101 /wd4251)
    endif()
    add_test(NAME ${name} COMMAND ${name}_test)
endmacro()
macro(wpilib_add_java_test name)
    cmake_parse_arguments(java_test "" "" "INCLUDE_JARS" ${ARGN})
    set(include_jars
        ${junit_jar}
        ${objenesis_jar}
        ${mockito_jar}
        ${byte_buddy_jar}
        ${byte_buddy_agent_jar}
        ${name}_jar
        ${java_test_INCLUDE_JARS}
    )
    file(GLOB_RECURSE JAVA_TEST_SOURCES src/test/java/*.java)
    add_jar(
        ${name}_test_jar
        ${JAVA_TEST_SOURCES}
        INCLUDE_JARS ${include_jars}
        OUTPUT_NAME ${name}-test
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
        ${java_test_UNPARSED_ARGUMENTS}
    )
    set(include_jars ${name}_test_jar ${include_jars})
    foreach(jar ${include_jars})
        if(TARGET ${jar})
            get_target_property(location ${jar} JAR_FILE)
            list(APPEND jars ${location})
        else()
            list(APPEND jars ${jar})
        endif()
    endforeach()
    if(MSVC)
        list(JOIN jars ";" classpath)
    else()
        list(JOIN jars ":" classpath)
    endif()
    get_target_property(location ${name}_test_jar JAR_FILE)
    add_test(
        NAME ${name}_java
        COMMAND
            java -cp "${classpath}" org.junit.platform.console.ConsoleLauncher
            --include-classname=.* --scan-class-path=${location} --fail-if-no-tests
        WORKING_DIRECTORY ${WPILIB_BINARY_DIR}
    )
    get_property(multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if(MSVC)
        set_tests_properties(
            ${name}_java
            PROPERTIES
                ENVIRONMENT_MODIFICATION
                    PATH=path_list_append:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<${multi_config}:$<CONFIG>>
        )
    elseif(APPLE)
        set_tests_properties(
            ${name}_java
            PROPERTIES
                ENVIRONMENT_MODIFICATION
                    DYLIB_LIBRARY_PATH=path_list_append:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$<${multi_config}:$<CONFIG>>
        )
    else()
        set_tests_properties(
            ${name}_java
            PROPERTIES
                ENVIRONMENT_MODIFICATION
                    LD_LIBRARY_PATH=path_list_append:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$<${multi_config}:$<CONFIG>>
        )
    endif()
endmacro()

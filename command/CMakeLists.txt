project(command)

include(SubDirList)
include(CompileWarnings)
include(AddTest)

if(WITH_JAVA)
    include(UseJava)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java src/generated/main/java/*.java)
    file(GLOB JACKSON_JARS "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson/*.jar")
    add_jar(
        command_jar
        ${JAVA_SOURCES}
        INCLUDE_JARS
            hal_jar
            ntcore_jar
            ${JACKSON_JARS}
            cscore_jar
            cameraserver_jar
            wpimath_jar
            wpiunits_jar
            wpiutil_jar
            wpilibj_jar
            datalog_jar
        OUTPUT_NAME command
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET command_jar PROPERTY FOLDER "java")

    install_jar(command_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS command_jar FILE command_jar.cmake DESTINATION share/command)
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    add_source_jar(
        command_src_jar
        BASE_DIRECTORIES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
            ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/java
        OUTPUT_NAME command-sources
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET command_src_jar PROPERTY FOLDER "java")

    install_jar(command_src_jar DESTINATION ${java_lib_dest})
endif()

file(GLOB_RECURSE command_native_src src/main/native/cpp/*.cpp src/generated/main/native/cpp/*.cpp)
add_library(command ${command_native_src})
set_target_properties(command PROPERTIES DEBUG_POSTFIX "d")
set_property(TARGET command PROPERTY FOLDER "libraries")

target_compile_features(command PUBLIC cxx_std_20)
wpilib_target_warnings(command)
target_link_libraries(command wpilibc)

target_include_directories(
    command
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/command>
)

install(TARGETS command EXPORT command)
export(TARGETS command FILE command.cmake NAMESPACE command::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/command")
install(DIRECTORY src/generated/main/native/include/ DESTINATION "${include_dest}/command")

configure_file(command-config.cmake.in ${WPILIB_BINARY_DIR}/command-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/command-config.cmake DESTINATION share/command)
install(EXPORT command DESTINATION share/command)

if(WITH_TESTS)
    wpilib_add_test(command src/test/native/cpp)
    target_include_directories(command_test PRIVATE src/test/native/include)
    target_link_libraries(command_test command googletest)
endif()

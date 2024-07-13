project(commandsv2)

include(SubDirList)
include(CompileWarnings)
include(AddTest)

if(WITH_JAVA)
    include(UseJava)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java src/generated/main/java/*.java)
    file(GLOB JACKSON_JARS "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson/*.jar")
    add_jar(
        commandsv2_jar
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
            wpiannotations_jar
        OUTPUT_NAME commandsv2
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET commandsv2_jar PROPERTY FOLDER "java")

    install_jar(commandsv2_jar DESTINATION ${java_lib_dest})
    install_jar_exports(
        TARGETS commandsv2_jar
        FILE commandsv2_jar.cmake
        DESTINATION share/commandsv2
    )
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    add_source_jar(
        commandsv2_src_jar
        BASE_DIRECTORIES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
            ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/java
        OUTPUT_NAME commandsv2-sources
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET commandsv2_src_jar PROPERTY FOLDER "java")

    install_jar(commandsv2_src_jar DESTINATION ${java_lib_dest})
endif()

file(
    GLOB_RECURSE commandsv2_native_src
    src/main/native/cpp/*.cpp
    src/generated/main/native/cpp/*.cpp
)
add_library(commandsv2 ${commandsv2_native_src})
set_target_properties(commandsv2 PROPERTIES DEBUG_POSTFIX "d")
set_property(TARGET commandsv2 PROPERTY FOLDER "libraries")

target_compile_features(commandsv2 PUBLIC cxx_std_23)
wpilib_target_warnings(commandsv2)
target_link_libraries(commandsv2 wpilibc)

target_include_directories(
    commandsv2
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/commandsv2>
)

install(TARGETS commandsv2 EXPORT commandsv2)
export(TARGETS commandsv2 FILE commandsv2.cmake NAMESPACE commandsv2::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/commandsv2")
install(DIRECTORY src/generated/main/native/include/ DESTINATION "${include_dest}/commandsv2")

configure_file(commandsv2-config.cmake.in ${WPILIB_BINARY_DIR}/commandsv2-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/commandsv2-config.cmake DESTINATION share/commandsv2)
install(EXPORT commandsv2 DESTINATION share/commandsv2)

if(WITH_TESTS)
    wpilib_add_test(commandsv2 src/test/native/cpp)
    target_include_directories(commandsv2_test PRIVATE src/test/native/include)
    target_link_libraries(commandsv2_test commandsv2 googletest)
endif()

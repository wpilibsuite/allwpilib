project(commandsv3)

include(SubDirList)
include(CompileWarnings)
include(AddTest)

if(WITH_JAVA)
    include(UseJava)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java src/generated/main/java/*.java)
    file(GLOB QUICKBUF_JAR ${WPILIB_BINARY_DIR}/wpiutil/thirdparty/quickbuf/*.jar)

    add_jar(
        commandsv3_jar
        ${JAVA_SOURCES}
        INCLUDE_JARS
            datalog_jar
            hal_jar
            ntcore_jar
            cscore_jar
            cameraserver_jar
            wpiannotations_jar
            wpimath_jar
            wpiunits_jar
            wpiutil_jar
            wpilibj_jar
            ${QUICKBUF_JAR}
        OUTPUT_NAME commandsv3
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )

    install_jar(commandsv3_jar DESTINATION ${java_lib_dest})
    install_jar_exports(
        TARGETS commandsv3_jar
        FILE commandsv3_jar.cmake
        DESTINATION share/commandsv3
    )
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    add_source_jar(
        commandsv3_src_jar
        BASE_DIRECTORIES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
            ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/java
        OUTPUT_NAME commandsv3-sources
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET commandsv3_src_jar PROPERTY FOLDER "java")
    install_jar(commandsv3_src_jar DESTINATION ${java_lib_dest})
endif()

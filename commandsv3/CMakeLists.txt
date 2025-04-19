project(commandsv3)

include(SubDirList)
include(CompileWarnings)
include(AddTest)

if(WITH_JAVA)
    find_package(Java REQUIRED)
    include(UseJava)
    set(CMAKE_JAVA_COMPILE_FLAGS "-encoding" "UTF8" "-Xlint:unchecked")

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
    add_jar(
        commandsv3_jar
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
        OUTPUT_NAME commandsv3
    )

    install_jar(commandsv3_jar DESTINATION ${java_lib_dest})
    install_jar_exports(
        TARGETS commandsv3_jar
        FILE commandsv3_jar.cmake
        DESTINATION share/commandsv3
    )
endif()

if(WITH_JAVA_SOURCE)
    find_package(Java REQUIRED)
    include(UseJava)
    file(GLOB commandsv3_SOURCES src/main/java/edu/wpi/first/wpilibj/commandsv3/*.java)
    file(GLOB commandsv3_BUTTON_SOURCES src/main/java/edu/wpi/first/wpilibj2/command/button*.java)
    add_jar(
        commandsv3_src_jar
        RESOURCES
        NAMESPACE "edu/wpi/first/wpilibj/commandsv3" ${commandsv3_SOURCES}
        NAMESPACE "edu/wpi/first/wpilibj/commandsv3/button" ${commandsv3_BUTTON_SOURCES}
        OUTPUT_NAME commandsv3-sources
    )

    get_property(commandsv3_SRC_JAR_FILE TARGET commandsv3_src_jar PROPERTY JAR_FILE)
    install(FILES ${commandsv3_SRC_JAR_FILE} DESTINATION "${java_lib_dest}")

    set_property(TARGET commandsv3_src_jar PROPERTY FOLDER "java")
endif()

install(TARGETS commandsv3 EXPORT commandsv3)
export(TARGETS commandsv3 FILE commandsv3.cmake NAMESPACE commandsv3::)

configure_file(commandsv3-config.cmake.in ${WPILIB_BINARY_DIR}/commandsv3-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/commandsv3-config.cmake DESTINATION share/commandsv3)
install(EXPORT commandsv3 DESTINATION share/commandsv3)

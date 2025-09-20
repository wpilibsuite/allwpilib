project(wpiannotations)

# Java bindings
if(WITH_JAVA)
    include(UseJava)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)

    add_jar(
        wpiannotations_jar
        ${JAVA_SOURCES}
        OUTPUT_NAME wpiannotations
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET wpiannotations_jar PROPERTY FOLDER "java")

    install_jar(wpiannotations_jar DESTINATION ${java_lib_dest})
    install_jar_exports(
        TARGETS wpiannotations_jar
        FILE wpiannotations.cmake
        DESTINATION share/wpiannotations
    )
    install(FILES wpiannotations-config.cmake DESTINATION share/wpiannotations)
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    add_source_jar(
        wpiannotations_src_jar
        BASE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
        OUTPUT_NAME wpiannotations-sources
    )
    set_property(TARGET wpiannotations_src_jar PROPERTY FOLDER "java")

    install_jar(wpiannotations_src_jar DESTINATION ${java_lib_dest})
endif()

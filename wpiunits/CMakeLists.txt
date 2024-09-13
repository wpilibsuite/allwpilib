project(wpiunits)

# Java bindings
if(WITH_JAVA)
    include(UseJava)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java src/generated/main/java/*.java)

    add_jar(
        wpiunits_jar
        ${JAVA_SOURCES}
        OUTPUT_NAME wpiunits
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET wpiunits_jar PROPERTY FOLDER "java")

    install_jar(wpiunits_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS wpiunits_jar FILE wpiunits.cmake DESTINATION share/wpiunits)
    install(FILES wpiunits-config.cmake DESTINATION share/wpiunits)
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    add_source_jar(
        wpiunits_src_jar
        BASE_DIRECTORIES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
            ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/java
        OUTPUT_NAME wpiunits-sources
    )
    set_property(TARGET wpiunits_src_jar PROPERTY FOLDER "java")

    install_jar(wpiunits_src_jar DESTINATION ${java_lib_dest})
endif()

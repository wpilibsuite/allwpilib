project(wpiunits)

# Java bindings
if(WITH_JAVA)
    find_package(Java REQUIRED)
    include(UseJava)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)

    add_jar(wpiunits_jar ${JAVA_SOURCES} OUTPUT_NAME wpiunits)
    set_property(TARGET wpiunits_jar PROPERTY FOLDER "java")

    install_jar(wpiunits_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS wpiunits_jar FILE wpiunits.cmake DESTINATION share/wpiunits)
    install(FILES wpiunits-config.cmake DESTINATION share/wpiunits)
endif()

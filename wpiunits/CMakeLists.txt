project (wpiunits)

# Java bindings
if (WITH_JAVA)
    find_package(Java REQUIRED)
    include(UseJava)
    set(CMAKE_JAVA_COMPILE_FLAGS "-encoding" "UTF8" "-Xlint:unchecked")

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)

    add_jar(wpiunits_jar ${JAVA_SOURCES} OUTPUT_NAME wpiunits)

    get_property(WPIUNITS_JAR_FILE TARGET wpiunits_jar PROPERTY JAR_FILE)
    install(FILES ${WPIUNITS_JAR_FILE} DESTINATION "${java_lib_dest}")

    set_property(TARGET wpiunits_jar PROPERTY FOLDER "java")

    if (WITH_FLAT_INSTALL)
        set (wpiunits_config_dir ${wpiunits_dest})
    else()
        set (wpiunits_config_dir share/wpiunits)
    endif()

    install(FILES wpiunits-config.cmake DESTINATION ${wpiunits_config_dir})
endif()

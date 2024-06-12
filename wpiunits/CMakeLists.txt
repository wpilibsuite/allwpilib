project(wpiunits)

# Java bindings
if(WITH_JAVA)
    find_package(Java REQUIRED)
    include(UseJava)
    set(CMAKE_JAVA_COMPILE_FLAGS "-encoding" "UTF8" "-Xlint:unchecked")

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)

    add_jar(wpiunits_jar ${JAVA_SOURCES} OUTPUT_NAME wpiunits)

    install_jar(wpiunits_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS wpiunits_jar FILE wpiunits.cmake DESTINATION share/wpiunits)
    install(FILES wpiunits-config.cmake DESTINATION share/wpiunits)
endif()

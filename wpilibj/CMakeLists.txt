project(wpilibj)

# Java bindings
if(WITH_JAVA)
    include(UseJava)

    set(OPENCV_JAVA_INSTALL_DIR ${OpenCV_INSTALL_PATH}/share/java/opencv4)

    find_file(
        OPENCV_JAR_FILE
        NAMES opencv-${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.jar
        PATHS
            ${OPENCV_JAVA_INSTALL_DIR}
            ${OpenCV_INSTALL_PATH}/bin
            ${OpenCV_INSTALL_PATH}/share/java
            ${OpenCV_INSTALL_PATH}/share/OpenCV/java
        NO_DEFAULT_PATH
    )

    configure_file(
        src/generate/WPILibVersion.java.in
        generated/main/java/edu/wpi/first/wpilibj/WPILibVersion.java
    )

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java src/generated/main/java/*.java)
    file(GLOB EJML_JARS "${WPILIB_BINARY_DIR}/wpimath/thirdparty/ejml/*.jar")
    file(GLOB JACKSON_JARS "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson/*.jar")

    add_jar(
        wpilibj_jar
        ${JAVA_SOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/generated/main/java/edu/wpi/first/wpilibj/WPILibVersion.java
        INCLUDE_JARS
            hal_jar
            ntcore_jar
            ${EJML_JARS}
            ${JACKSON_JARS}
            ${OPENCV_JAR_FILE}
            cscore_jar
            cameraserver_jar
            wpimath_jar
            wpiunits_jar
            wpiutil_jar
        OUTPUT_NAME wpilibj
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET wpilibj_jar PROPERTY FOLDER "java")

    install_jar(wpilibj_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS wpilibj_jar FILE wpilibj.cmake DESTINATION share/wpilibj)

    install(FILES wpilibj-config.cmake DESTINATION share/wpilibj)
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    # Generate version file if it wasn't generated already
    if(NOT WITH_JAVA)
        configure_file(
            src/generate/WPILibVersion.java.in
            generated/main/java/edu/wpi/first/wpilibj/WPILibVersion.java
        )
    endif()
    add_source_jar(
        wpilibj_src_jar
        BASE_DIRECTORIES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
            ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/java
            ${CMAKE_CURRENT_BINARY_DIR}/generated/main/java
        OUTPUT_NAME wpilibj-sources
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET wpilibj_src_jar PROPERTY FOLDER "java")

    install_jar(wpilibj_src_jar DESTINATION ${java_lib_dest})
endif()

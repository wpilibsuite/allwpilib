project (wpilibj)

# Java bindings
if (WITH_JAVA)
    find_package( OpenCV REQUIRED )
    find_package(Java REQUIRED)
    include(UseJava)
    set(CMAKE_JAVA_COMPILE_FLAGS "-encoding" "UTF8" "-Xlint:unchecked")

    set(OPENCV_JAVA_INSTALL_DIR ${OpenCV_INSTALL_PATH}/share/OpenCV/java/)

    find_file(OPENCV_JAR_FILE NAMES opencv-${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.jar PATHS ${OPENCV_JAVA_INSTALL_DIR} ${OpenCV_INSTALL_PATH}/bin NO_DEFAULT_PATH)

    configure_file(src/generate/WPILibVersion.java.in WPILibVersion.java)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
    file(GLOB EJML_JARS "${WPILIB_BINARY_DIR}/wpimath/thirdparty/ejml/*.jar")
    file(GLOB JACKSON_JARS "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson/*.jar")

    add_jar(wpilibj_jar ${JAVA_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.java INCLUDE_JARS hal_jar ntcore_jar ${EJML_JARS} ${JACKSON_JARS} ${OPENCV_JAR_FILE} cscore_jar cameraserver_jar wpimath_jar wpiutil_jar OUTPUT_NAME wpilibj)

    get_property(WPILIBJ_JAR_FILE TARGET wpilibj_jar PROPERTY JAR_FILE)
    install(FILES ${WPILIBJ_JAR_FILE} DESTINATION "${java_lib_dest}")

    set_property(TARGET wpilibj_jar PROPERTY FOLDER "java")

    if (WITH_FLAT_INSTALL)
        set (wpilibj_config_dir ${wpilib_dest})
    else()
        set (wpilibj_config_dir share/wpilibj)
    endif()

    install(FILES wpilibj-config.cmake DESTINATION ${wpilibj_config_dir})
endif()

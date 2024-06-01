project(wpilibj)

# Java bindings
if(WITH_JAVA)
    find_package(OpenCV REQUIRED)
    find_package(Java REQUIRED)
    include(UseJava)

    set(OPENCV_JAVA_INSTALL_DIR ${OpenCV_INSTALL_PATH}/share/OpenCV/java/)

    find_file(
        OPENCV_JAR_FILE
        NAMES opencv-${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.jar
        PATHS ${OPENCV_JAVA_INSTALL_DIR} ${OpenCV_INSTALL_PATH}/bin
        NO_DEFAULT_PATH
    )

    configure_file(src/generate/WPILibVersion.java.in WPILibVersion.java)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
    file(GLOB EJML_JARS "${WPILIB_BINARY_DIR}/wpimath/thirdparty/ejml/*.jar")
    file(GLOB JACKSON_JARS "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson/*.jar")

    add_jar(
        wpilibj_jar
        ${JAVA_SOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.java
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
    )
    set_property(TARGET wpilibj_jar PROPERTY FOLDER "java")

    install_jar(wpilibj_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS wpilibj_jar FILE wpilibj.cmake DESTINATION share/wpilibj)

    install(FILES wpilibj-config.cmake DESTINATION share/wpilibj)
endif()

if(WITH_JAVA_SOURCE)
    find_package(Java REQUIRED)
    include(UseJava)
    file(GLOB WPILIBJ_SOURCES src/main/java/edu/wpi/first/wpilibj/*.java)
    file(GLOB WPILIBJ_COUNTER_SOURCES src/main/java/edu/wpi/first/wpilibj/counter/*.java)
    file(GLOB WPILIBJ_DRIVE_SOURCES src/main/java/edu/wpi/first/wpilibj/drive/*.java)
    file(GLOB WPILIBJ_EVENT_SOURCES src/main/java/edu/wpi/first/wpilibj/event/*.java)
    file(GLOB WPILIBJ_INTERFACES_SOURCES src/main/java/edu/wpi/first/wpilibj/interfaces/*.java)
    file(GLOB WPILIBJ_MOTORCONTROL_SOURCES src/main/java/edu/wpi/first/wpilibj/motorcontrol*.java)
    file(GLOB WPILIBJ_SHUFFLEBOARD_SOURCES src/main/java/edu/wpi/first/wpilibj/shuffleboard*.java)
    file(GLOB WPILIBJ_SIMULATION_SOURCES src/main/java/edu/wpi/first/wpilibj/simulation*.java)
    file(GLOB WPILIBJ_SMARTDASHBOARD_SOURCES src/main/java/edu/wpi/first/wpilibj/*.java)
    file(
        GLOB WPILIBJ_UTIL_SOURCES
        src/main/java/edu/wpi/first/wpilibj/*.java
        ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.java
    )
    add_jar(
        wpilibj_src_jar
        RESOURCES
        NAMESPACE "edu/wpi/first/wpilibj" ${WPILIBJ_SOURCES}
        NAMESPACE "edu/wpi/first/wpilibj/counter" ${WPILIBJ_COUNTER_SOURCES}
        NAMESPACE "edu/wpi/first/wpilibj/drive" ${WPILIBJ_DRIVE_SOURCES}
        NAMESPACE "edu/wpi/first/wpilibj/event" ${WPILIBJ_EVENT_SOURCES}
        NAMESPACE "edu/wpi/first/wpilibj/interfaces" ${WPILIBJ_INTERFACES_SOURCES}
        NAMESPACE
            "edu/wpi/first/wpilibj/internal"
            src/main/java/edu/wpi/first/wpilibj/internal/DriverStationModeThread.java
        NAMESPACE
            "edu/wpi/first/wpilibj/livewindow"
            src/main/java/edu/wpi/first/wpilibj/livewindow/LiveWindow.java
        NAMESPACE "edu/wpi/first/wpilibj/motorcontrol" ${WPILIBJ_MOTORCONTROL_SOURCES}
        NAMESPACE "edu/wpi/first/wpilibj/shuffleboard" ${WPILIBJ_SHUFFLEBOARD_SOURCES}
        NAMESPACE "edu/wpi/first/wpilibj/simulation" ${WPILIBJ_SIMULATION_SOURCES}
        NAMESPACE "edu/wpi/first/wpilibj/smartdashboard" ${WPILIBJ_SMARTDASHBOARD_SOURCES}
        NAMESPACE "edu/wpi/first/wpilibj/util" ${WPILIBJ_UTIL_SOURCES}
        OUTPUT_NAME wpilibj-sources
    )

    get_property(WPILIBJ_SRC_JAR_FILE TARGET wpilibj_src_jar PROPERTY JAR_FILE)
    install(FILES ${WPILIBJ_SRC_JAR_FILE} DESTINATION "${java_lib_dest}")

    set_property(TARGET wpilibj_src_jar PROPERTY FOLDER "java")
endif()

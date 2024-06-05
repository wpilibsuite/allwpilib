project(cameraservercv)

include(CompileWarnings)
include(AddTest)

find_package(OpenCV REQUIRED)

# Java bindings
if(WITH_JAVA)
    find_package(Java REQUIRED)
    include(UseJava)
    set(CMAKE_JAVA_COMPILE_FLAGS "-encoding" "UTF8" "-Xlint:unchecked")

    #find java files, copy them locally

    set(OPENCV_JAVA_INSTALL_DIR ${OpenCV_INSTALL_PATH}/share/OpenCV/java/)

    find_file(
        OPENCV_JAR_FILE
        NAMES opencv-${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.jar
        PATHS ${OPENCV_JAVA_INSTALL_DIR} ${OpenCV_INSTALL_PATH}/bin
        NO_DEFAULT_PATH
    )

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)

    add_jar(
        cameraservercv_jar
        ${JAVA_SOURCES}
        INCLUDE_JARS wpiutil_jar cameraserver_jar cscorecv_jar cscore_jar ntcore_jar ${OPENCV_JAR_FILE}
        OUTPUT_NAME cameraservercv
    )

    install_jar(cameraservercv_jar DESTINATION ${java_lib_dest})
    install_jar_exports(
        TARGETS cameraservercv_jar
        FILE cameraservercv_jar.cmake
        DESTINATION share/cameraservercv
    )
endif()

if(WITH_JAVA_SOURCE)
    find_package(Java REQUIRED)
    include(UseJava)
    file(GLOB CAMERASERVERCV_SOURCES src/main/java/edu/wpi/first/cameraservercv/*.java)
    file(GLOB VISION_SOURCES src/main/java/edu/wpi/first/vision/*.java)
    add_jar(
        cameraservercv_src_jar
        RESOURCES
        NAMESPACE "edu/wpi/first/cameraservercv" ${CAMERASERVERCV_SOURCES}
        NAMESPACE "edu/wpi/first/vision" ${VISION_SOURCES}
        OUTPUT_NAME cameraservercv-sources
    )

    get_property(CAMERASERVERCV_SRC_JAR_FILE TARGET cameraservercv_src_jar PROPERTY JAR_FILE)
    install(FILES ${CAMERASERVERCV_SRC_JAR_FILE} DESTINATION "${java_lib_dest}")

    set_property(TARGET cameraservercv_src_jar PROPERTY FOLDER "java")
endif()

file(GLOB_RECURSE cameraservercv_native_src src/main/native/cpp/*.cpp)
add_library(cameraservercv ${cameraservercv_native_src})
set_target_properties(cameraservercv PROPERTIES DEBUG_POSTFIX "d")
target_include_directories(
    cameraservercv
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/cameraservercv>
)
wpilib_target_warnings(cameraservercv)
target_link_libraries(
    cameraservercv
    PUBLIC ntcore cameraserver cscorecv cscore wpiutil ${OpenCV_LIBS}
)

set_property(TARGET cameraservercv PROPERTY FOLDER "libraries")

install(TARGETS cameraservercv EXPORT cameraservercv)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/cameraservercv")

configure_file(cameraservercv-config.cmake.in ${WPILIB_BINARY_DIR}/cameraservercv-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/cameraservercv-config.cmake DESTINATION share/cameraservercv)
install(EXPORT cameraservercv DESTINATION share/cameraservercv)

if(WITH_TESTS)
    wpilib_add_test(cameraservercv src/test/native/cpp)
    target_link_libraries(cameraservercv_test cameraserver gtest)
endif()

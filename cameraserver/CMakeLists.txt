project(cameraserver)

include(CompileWarnings)
include(AddTest)

# Java bindings
if(WITH_JAVA)
    include(UseJava)

    #find JAR file, copy it locally
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

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)

    add_jar(
        cameraserver_jar
        ${JAVA_SOURCES}
        INCLUDE_JARS wpiutil_jar cscore_jar ntcore_jar ${OPENCV_JAR_FILE}
        OUTPUT_NAME cameraserver
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET cameraserver_jar PROPERTY FOLDER "java")

    install_jar(cameraserver_jar DESTINATION ${java_lib_dest})
    install_jar_exports(
        TARGETS cameraserver_jar
        FILE cameraserver_jar.cmake
        DESTINATION share/cameraserver
    )
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    add_source_jar(
        cameraserver_src_jar
        BASE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
        OUTPUT_NAME cameraserver-sources
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET cameraserver_src_jar PROPERTY FOLDER "java")

    install_jar(cameraserver_src_jar DESTINATION ${java_lib_dest})
endif()

file(GLOB_RECURSE cameraserver_native_src src/main/native/cpp/*.cpp)
add_library(cameraserver ${cameraserver_native_src})
set_target_properties(cameraserver PROPERTIES DEBUG_POSTFIX "d")
target_include_directories(
    cameraserver
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/cameraserver>
)
wpilib_target_warnings(cameraserver)
target_link_libraries(cameraserver PUBLIC ntcore cscore wpiutil ${OpenCV_LIBS})

set_property(TARGET cameraserver PROPERTY FOLDER "libraries")

install(TARGETS cameraserver EXPORT cameraserver)
export(TARGETS cameraserver FILE cameraserver.cmake NAMESPACE cameraserver::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/cameraserver")

configure_file(cameraserver-config.cmake.in ${WPILIB_BINARY_DIR}/cameraserver-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/cameraserver-config.cmake DESTINATION share/cameraserver)
install(EXPORT cameraserver DESTINATION share/cameraserver)

file(GLOB multiCameraServer_src multiCameraServer/src/main/native/cpp/*.cpp)
add_executable(multiCameraServer ${multiCameraServer_src})
wpilib_target_warnings(multiCameraServer)
target_link_libraries(multiCameraServer cameraserver)

set_property(TARGET multiCameraServer PROPERTY FOLDER "examples")

if(WITH_TESTS)
    wpilib_add_test(cameraserver src/test/native/cpp)
    target_link_libraries(cameraserver_test cameraserver googletest)
endif()

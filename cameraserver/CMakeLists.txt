project(cameraserver)

include(CompileWarnings)
include(AddTest)

find_package( OpenCV REQUIRED )

# Java bindings
if (WITH_JAVA)
    find_package(Java REQUIRED)
    include(UseJava)
    set(CMAKE_JAVA_COMPILE_FLAGS "-Xlint:unchecked")

    #find java files, copy them locally

    set(OPENCV_JAVA_INSTALL_DIR ${OpenCV_INSTALL_PATH}/share/OpenCV/java/)

    find_file(OPENCV_JAR_FILE NAMES opencv-${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.jar PATHS ${OPENCV_JAVA_INSTALL_DIR} ${OpenCV_INSTALL_PATH}/bin NO_DEFAULT_PATH)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)

    add_jar(cameraserver_jar ${JAVA_SOURCES} INCLUDE_JARS wpiutil_jar cscore_jar ntcore_jar ${OPENCV_JAR_FILE} OUTPUT_NAME cameraserver)

    get_property(CAMERASERVER_JAR_FILE TARGET cameraserver_jar PROPERTY JAR_FILE)
    install(FILES ${CAMERASERVER_JAR_FILE} DESTINATION "${java_lib_dest}")

    set_property(TARGET cameraserver_jar PROPERTY FOLDER "java")

endif()

file(GLOB_RECURSE
    cameraserver_native_src src/main/native/cpp/*.cpp)
add_library(cameraserver ${cameraserver_native_src})
set_target_properties(cameraserver PROPERTIES DEBUG_POSTFIX "d")
target_include_directories(cameraserver PUBLIC
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
                            $<INSTALL_INTERFACE:${include_dest}/cameraserver>)
wpilib_target_warnings(cameraserver)
target_link_libraries(cameraserver PUBLIC ntcore cscore wpiutil ${OpenCV_LIBS})

set_property(TARGET cameraserver PROPERTY FOLDER "libraries")

install(TARGETS cameraserver EXPORT cameraserver DESTINATION "${main_lib_dest}")
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/cameraserver")

if (WITH_JAVA AND MSVC)
    install(TARGETS cameraserver RUNTIME DESTINATION "${jni_lib_dest}" COMPONENT Runtime)
endif()

if (WITH_FLAT_INSTALL)
    set (cameraserver_config_dir ${wpilib_dest})
else()
    set (cameraserver_config_dir share/cameraserver)
endif()

configure_file(cameraserver-config.cmake.in ${WPILIB_BINARY_DIR}/cameraserver-config.cmake )
install(FILES ${WPILIB_BINARY_DIR}/cameraserver-config.cmake DESTINATION ${cameraserver_config_dir})
install(EXPORT cameraserver DESTINATION ${cameraserver_config_dir})

file(GLOB multiCameraServer_src multiCameraServer/src/main/native/cpp/*.cpp)
add_executable(multiCameraServer ${multiCameraServer_src})
wpilib_target_warnings(multiCameraServer)
target_link_libraries(multiCameraServer cameraserver)

set_property(TARGET multiCameraServer PROPERTY FOLDER "examples")

if (WITH_TESTS)
    wpilib_add_test(cameraserver src/test/native/cpp)
    target_link_libraries(cameraserver_test cameraserver gtest)
endif()

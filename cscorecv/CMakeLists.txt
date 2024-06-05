project(cscorecv)

include(SubDirList)
include(CompileWarnings)
include(AddTest)
include(LinkMacOSGUI)

find_package(OpenCV REQUIRED)

file(GLOB cscorecv_native_src src/main/native/cpp/*.cpp)

add_library(cscorecv ${cscorecv_native_src})
set_target_properties(cscorecv PROPERTIES DEBUG_POSTFIX "d")

if(NOT MSVC)
    if(APPLE)
        set_target_properties(
            cscorecv
            PROPERTIES
                LINK_FLAGS
                    "-framework CoreFoundation -framework AVFoundation -framework Foundation -framework CoreMedia -framework CoreVideo"
        )
    endif()
else()
    target_compile_definitions(cscorecv PUBLIC -DNOMINMAX)
    target_compile_definitions(cscorecv PRIVATE -D_CRT_SECURE_NO_WARNINGS)
endif()

target_include_directories(
    cscorecv
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/cscorecv>
)
target_include_directories(cscorecv PRIVATE src/main/native/cpp)
wpilib_target_warnings(cscorecv)
target_link_libraries(cscorecv PUBLIC cscore wpinet wpiutil ${OpenCV_LIBS})

set_property(TARGET cscorecv PROPERTY FOLDER "libraries")

install(TARGETS cscorecv EXPORT cscorecv)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/cscorecv")

configure_file(cscorecv-config.cmake.in ${WPILIB_BINARY_DIR}/cscorecv-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/cscorecv-config.cmake DESTINATION share/cscorecv)
install(EXPORT cscorecv DESTINATION share/cscorecv)

subdir_list(cscore_examples "${CMAKE_CURRENT_SOURCE_DIR}/examples")
foreach(example ${cscore_examples})
    file(GLOB cscore_example_src examples/${example}/*.cpp)
    unset(add_libs)
    if(${example} STREQUAL "usbviewer")
        if(TARGET wpigui)
            set(add_libs wpigui)
        else()
            unset(cscore_example_src)
        endif()
    endif()
    if(cscore_example_src)
        add_executable(cscorecv_${example} ${cscore_example_src})
        wpilib_target_warnings(cscorecv_${example})

        if(${example} STREQUAL "usbviewer")
            wpilib_link_macos_gui(cscorecv_${example})
        endif()

        target_link_libraries(cscorecv_${example} cscorecv ${add_libs})
        set_property(TARGET cscorecv_${example} PROPERTY FOLDER "examples")
    endif()
endforeach()

# Java bindings
if(WITH_JAVA)
    find_package(Java REQUIRED)
    find_package(JNI REQUIRED)
    include(UseJava)
    set(CMAKE_JAVA_COMPILE_FLAGS "-encoding" "UTF8" "-Xlint:unchecked")

    #find java files, copy them locally

    if("${OPENCV_JAVA_INSTALL_DIR}" STREQUAL "")
        set(OPENCV_JAVA_INSTALL_DIR ${OpenCV_INSTALL_PATH}/share/OpenCV/java/)
    endif()

    find_file(
        OPENCV_JAR_FILE
        NAMES opencv-${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.jar
        PATHS
            ${OPENCV_JAVA_INSTALL_DIR}
            ${OpenCV_INSTALL_PATH}/bin
            ${OpenCV_INSTALL_PATH}/share/java
        NO_DEFAULT_PATH
    )
    find_file(
        OPENCV_JNI_FILE
        NAMES
            libopencv_java${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.so
            libopencv_java${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dylib
            opencv_java${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll
        PATHS
            ${OPENCV_JAVA_INSTALL_DIR}
            ${OpenCV_INSTALL_PATH}/bin
            ${OpenCV_INSTALL_PATH}/bin/Release
            ${OpenCV_INSTALL_PATH}/bin/Debug
            ${OpenCV_INSTALL_PATH}/lib
            ${OpenCV_INSTALL_PATH}/lib/jni
        NO_DEFAULT_PATH
    )

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
    set(CMAKE_JNI_TARGET true)

    add_jar(
        cscorecv_jar
        ${JAVA_SOURCES}
        INCLUDE_JARS cscore_jar wpiutil_jar ${OPENCV_JAR_FILE}
        OUTPUT_NAME cscorecv
    )

    install_jar(cscorecv_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS cscorecv_jar FILE cscorecv_jar.cmake DESTINATION share/cscorecv)
    install(FILES ${OPENCV_JAR_FILE} DESTINATION "${java_lib_dest}")

    if(MSVC)
        install(FILES ${OPENCV_JNI_FILE} DESTINATION "${jni_lib_dest}")

        foreach(cvFile ${OpenCV_LIBS})
            find_file(
                ${cvFile}Loc
                NAMES
                    ${cvFile}${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll
                PATHS
                    ${OPENCV_JAVA_INSTALL_DIR}
                    ${OpenCV_INSTALL_PATH}/bin
                    ${OpenCV_INSTALL_PATH}/bin/Release
                    ${OpenCV_INSTALL_PATH}/bin/Debug
                    ${OpenCV_INSTALL_PATH}/lib
                NO_DEFAULT_PATH
            )
            install(FILES ${${cvFile}Loc} DESTINATION "${jni_lib_dest}")
        endforeach()
    endif()

endif()

if(WITH_JAVA_SOURCE)
    find_package(Java REQUIRED)
    include(UseJava)
    file(GLOB CSCORECV_SOURCES src/main/java/edu/wpi/first/cscorecv/*.java)
    add_jar(
        cscorecv_src_jar
        RESOURCES
        NAMESPACE "edu/wpi/first/cscorecv" ${CSCORECV_SOURCES}
        OUTPUT_NAME cscorecv-sources
    )

    get_property(CSCORECV_SRC_JAR_FILE TARGET cscorecv_src_jar PROPERTY JAR_FILE)
    install(FILES ${CSCORECV_SRC_JAR_FILE} DESTINATION "${java_lib_dest}")

    set_property(TARGET cscorecv_src_jar PROPERTY FOLDER "java")
endif()

if(WITH_TESTS)
    wpilib_add_test(cscorecv src/test/native/cpp)
    target_link_libraries(cscorecv_test cscorecv gmock)
endif()

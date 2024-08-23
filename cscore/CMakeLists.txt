project(cscore)

include(SubDirList)
include(CompileWarnings)
include(AddTest)
include(LinkMacOSGUI)

file(GLOB cscore_native_src src/main/native/cpp/*.cpp)
file(GLOB cscore_linux_src src/main/native/linux/*.cpp)
file(GLOB cscore_osx_src src/main/native/osx/*.cpp)
file(GLOB cscore_osx_objc_src src/main/native/objcpp/*.mm)
file(GLOB cscore_windows_src src/main/native/windows/*.cpp)

add_library(cscore ${cscore_native_src})
set_target_properties(cscore PROPERTIES DEBUG_POSTFIX "d")

if(APPLE)
    target_sources(cscore PRIVATE ${cscore_osx_src} ${cscore_osx_objc_src})
    target_compile_options(cscore PRIVATE "-fobjc-arc")
    set_target_properties(
        cscore
        PROPERTIES
            LINK_FLAGS
                "-framework CoreFoundation -framework AVFoundation -framework Foundation -framework CoreMedia -framework CoreVideo"
    )
elseif(MSVC)
    target_sources(cscore PRIVATE ${cscore_windows_src})
    target_compile_definitions(cscore PUBLIC -DNOMINMAX)
    target_compile_definitions(cscore PRIVATE -D_CRT_SECURE_NO_WARNINGS)
else()
    target_sources(cscore PRIVATE ${cscore_linux_src})
endif()

target_include_directories(
    cscore
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/cscore>
    PRIVATE src/main/native/cpp
)
wpilib_target_warnings(cscore)
target_link_libraries(cscore PUBLIC wpinet wpiutil ${OpenCV_LIBS})

set_property(TARGET cscore PROPERTY FOLDER "libraries")

install(TARGETS cscore EXPORT cscore)
export(TARGETS cscore FILE cscore.cmake NAMESPACE cscore::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/cscore")

configure_file(cscore-config.cmake.in ${WPILIB_BINARY_DIR}/cscore-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/cscore-config.cmake DESTINATION share/cscore)
install(EXPORT cscore DESTINATION share/cscore)

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
        add_executable(cscore_${example} ${cscore_example_src})
        wpilib_target_warnings(cscore_${example})

        if(${example} STREQUAL "usbviewer")
            wpilib_link_macos_gui(cscore_${example})
        endif()

        target_link_libraries(cscore_${example} cscore ${add_libs})
        set_property(TARGET cscore_${example} PROPERTY FOLDER "examples")
    endif()
endforeach()

# Java bindings
if(WITH_JAVA)
    include(UseJava)

    #find JAR file, copy it locally
    if("${OPENCV_JAVA_INSTALL_DIR}" STREQUAL "")
        set(OPENCV_JAVA_INSTALL_DIR ${OpenCV_INSTALL_PATH}/share/java/opencv4)
    endif()

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

    file(GLOB cscore_jni_src src/main/native/cpp/jni/CameraServerJNI.cpp)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
    set(CMAKE_JNI_TARGET true)

    add_jar(
        cscore_jar
        ${JAVA_SOURCES}
        INCLUDE_JARS wpiutil_jar ${OPENCV_JAR_FILE}
        OUTPUT_NAME cscore
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
        GENERATE_NATIVE_HEADERS cscore_jni_headers
    )
    set_property(TARGET cscore_jar PROPERTY FOLDER "java")

    install_jar(cscore_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS cscore_jar FILE cscore_jar.cmake DESTINATION share/cscore)
    install(FILES ${OPENCV_JAR_FILE} DESTINATION "${java_lib_dest}")

    add_library(cscorejni ${cscore_jni_src})
    wpilib_target_warnings(cscorejni)
    target_link_libraries(cscorejni PUBLIC cscore wpiutil ${OpenCV_LIBS})

    set_property(TARGET cscorejni PROPERTY FOLDER "libraries")

    target_link_libraries(cscorejni PRIVATE cscore_jni_headers)
    add_dependencies(cscorejni cscore_jar)

    install(TARGETS cscorejni EXPORT cscorejni)
    export(TARGETS cscorejni FILE cscorejni.cmake NAMESPACE cscorejni::)
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    add_source_jar(
        cscore_src_jar
        BASE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
        OUTPUT_NAME cscore-sources
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET cscore_src_jar PROPERTY FOLDER "java")

    install_jar(cscore_src_jar DESTINATION ${java_lib_dest})
endif()

if(WITH_TESTS)
    wpilib_add_test(cscore src/test/native/cpp)
    target_link_libraries(cscore_test cscore googletest)
endif()

project(ntcore)

include(CompileWarnings)
include(AddTest)

file(
    GLOB ntcore_native_src
    src/main/native/cpp/*.cpp
    src/generated/main/native/cpp/*.cpp
    src/main/native/cpp/local/*.cpp
    src/main/native/cpp/net/*.cpp
    src/main/native/cpp/net3/*.cpp
    src/main/native/cpp/networktables/*.cpp
    src/main/native/cpp/server/*.cpp
    src/main/native/cpp/tables/*.cpp
)
add_library(ntcore ${ntcore_native_src})
set_target_properties(ntcore PROPERTIES DEBUG_POSTFIX "d")
target_include_directories(
    ntcore
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/cpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/ntcore>
)
wpilib_target_warnings(ntcore)
target_compile_features(ntcore PUBLIC cxx_std_20)
target_link_libraries(ntcore PUBLIC wpinet wpiutil)

set_property(TARGET ntcore PROPERTY FOLDER "libraries")

install(TARGETS ntcore EXPORT ntcore)
export(TARGETS ntcore FILE ntcore.cmake NAMESPACE ntcore::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/ntcore")
install(DIRECTORY src/generated/main/native/include/ DESTINATION "${include_dest}/ntcore")

configure_file(ntcore-config.cmake.in ${WPILIB_BINARY_DIR}/ntcore-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/ntcore-config.cmake DESTINATION share/ntcore)
install(EXPORT ntcore DESTINATION share/ntcore)

# Java bindings
if(WITH_JAVA)
    include(UseJava)

    file(GLOB QUICKBUF_JAR ${WPILIB_BINARY_DIR}/wpiutil/thirdparty/quickbuf/*.jar)

    file(GLOB ntcore_jni_src src/main/native/cpp/jni/*.cpp src/generated/main/native/cpp/jni/*.cpp)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java src/generated/main/java/*.java)
    set(CMAKE_JNI_TARGET true)

    add_jar(
        ntcore_jar
        ${JAVA_SOURCES}
        INCLUDE_JARS wpiutil_jar ${QUICKBUF_JAR}
        OUTPUT_NAME ntcore
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
        GENERATE_NATIVE_HEADERS ntcore_jni_headers
    )
    set_property(TARGET ntcore_jar PROPERTY FOLDER "java")

    install_jar(ntcore_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS ntcore_jar FILE ntcore_jar.cmake DESTINATION share/ntcore)

    add_library(ntcorejni ${ntcore_jni_src})
    wpilib_target_warnings(ntcorejni)
    target_link_libraries(ntcorejni PUBLIC ntcore wpiutil)

    set_property(TARGET ntcorejni PROPERTY FOLDER "libraries")

    target_link_libraries(ntcorejni PRIVATE ntcore_jni_headers)
    add_dependencies(ntcorejni ntcore_jar)

    install(TARGETS ntcorejni EXPORT ntcorejni)
    export(TARGETS ntcorejni FILE ntcorejni.cmake NAMESPACE ntcorejni::)
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    add_source_jar(
        ntcore_src_jar
        BASE_DIRECTORIES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
            ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/java
        OUTPUT_NAME ntcore-sources
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET ntcore_src_jar PROPERTY FOLDER "java")

    install_jar(ntcore_src_jar DESTINATION ${java_lib_dest})
endif()

add_executable(ntcoredev src/dev/native/cpp/main.cpp)
wpilib_target_warnings(ntcoredev)
target_link_libraries(ntcoredev ntcore)

if(WITH_TESTS)
    wpilib_add_test(ntcore src/test/native/cpp)
    target_include_directories(ntcore_test PRIVATE src/main/native/cpp)
    target_link_libraries(ntcore_test ntcore googletest wpiutil_testlib)
endif()

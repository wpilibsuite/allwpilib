project(hal)

include(CompileWarnings)
include(AddTest)

file(
    GLOB hal_shared_native_src
    src/main/native/cpp/*.cpp
    src/main/native/cpp/cpp/*.cpp
    src/main/native/cpp/handles/*.cpp
)
file(GLOB_RECURSE hal_sim_native_src src/main/native/sim/*.cpp)
add_library(hal ${hal_shared_native_src} ${hal_sim_native_src})
wpilib_target_warnings(hal)
set_target_properties(hal PROPERTIES DEBUG_POSTFIX "d")
set_target_properties(hal PROPERTIES OUTPUT_NAME "wpiHal")

target_include_directories(
    hal
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/hal>
)
target_link_libraries(hal PUBLIC wpiutil)

set_property(TARGET hal PROPERTY FOLDER "libraries")

install(TARGETS hal EXPORT hal)
export(TARGETS hal FILE hal.cmake NAMESPACE hal::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/hal")
install(DIRECTORY src/generated/main/native/include/ DESTINATION "${include_dest}/hal")

configure_file(hal-config.cmake.in ${WPILIB_BINARY_DIR}/hal-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/hal-config.cmake DESTINATION share/hal)
install(EXPORT hal DESTINATION share/hal)

# Java bindings
if(WITH_JAVA)
    include(UseJava)

    file(GLOB_RECURSE hal_shared_jni_src src/main/native/cpp/jni/*.cpp)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java src/generated/main/java/*.java)
    set(CMAKE_JNI_TARGET true)

    add_jar(
        hal_jar
        ${JAVA_SOURCES}
        INCLUDE_JARS wpiutil_jar
        OUTPUT_NAME wpiHal
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
        GENERATE_NATIVE_HEADERS hal_jni_headers
    )
    set_property(TARGET hal_jar PROPERTY FOLDER "java")

    install_jar(hal_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS hal_jar FILE hal_jar.cmake DESTINATION share/hal)

    add_library(haljni ${hal_shared_jni_src})
    set_target_properties(haljni PROPERTIES OUTPUT_NAME "wpiHaljni")

    wpilib_target_warnings(haljni)
    target_link_libraries(haljni PUBLIC hal wpiutil)

    set_property(TARGET haljni PROPERTY FOLDER "libraries")

    target_link_libraries(haljni PRIVATE hal_jni_headers)
    add_dependencies(haljni hal_jar)

    install(TARGETS haljni EXPORT haljni)
    export(TARGETS haljni FILE haljni.cmake NAMESPACE haljni::)
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    add_source_jar(
        hal_src_jar
        BASE_DIRECTORIES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
            ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/java
        OUTPUT_NAME wpiHal-sources
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET hal_src_jar PROPERTY FOLDER "java")

    install_jar(hal_src_jar DESTINATION ${java_lib_dest})
endif()

if(WITH_TESTS)
    wpilib_add_test(hal src/test/native/cpp)
    target_link_libraries(hal_test hal googletest)
endif()

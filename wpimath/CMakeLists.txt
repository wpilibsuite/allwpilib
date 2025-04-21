project(wpimath)

include(SubDirList)
include(CompileWarnings)
include(AddTest)
include(DownloadAndCheck)

file(
    GLOB wpimath_jni_src
    src/main/native/cpp/jni/ArmFeedforwardJNI.cpp
    src/main/native/cpp/jni/DAREJNI.cpp
    src/main/native/cpp/jni/EigenJNI.cpp
    src/main/native/cpp/jni/Ellipse2dJNI.cpp
    src/main/native/cpp/jni/Exceptions.cpp
    src/main/native/cpp/jni/Pose3dJNI.cpp
    src/main/native/cpp/jni/StateSpaceUtilJNI.cpp
    src/main/native/cpp/jni/TrajectoryUtilJNI.cpp
)

# Java bindings
if(WITH_JAVA)
    include(UseJava)

    if(NOT EXISTS "${WPILIB_BINARY_DIR}/wpimath/thirdparty/ejml/ejml-simple-0.43.1.jar")
        set(BASE_URL "https://search.maven.org/remotecontent?filepath=")
        set(JAR_ROOT "${WPILIB_BINARY_DIR}/wpimath/thirdparty/ejml")

        message(STATUS "Downloading EJML jarfiles...")

        download_and_check(
            "${BASE_URL}org/ejml/ejml-cdense/0.43.1/ejml-cdense-0.43.1.jar"
            "${JAR_ROOT}/ejml-cdense-0.43.1.jar"
        )
        download_and_check(
            "${BASE_URL}org/ejml/ejml-core/0.43.1/ejml-core-0.43.1.jar"
            "${JAR_ROOT}/ejml-core-0.43.1.jar"
        )
        download_and_check(
            "${BASE_URL}org/ejml/ejml-ddense/0.43.1/ejml-ddense-0.43.1.jar"
            "${JAR_ROOT}/ejml-ddense-0.43.1.jar"
        )
        download_and_check(
            "${BASE_URL}org/ejml/ejml-dsparse/0.43.1/ejml-dsparse-0.43.1.jar"
            "${JAR_ROOT}/ejml-dsparse-0.43.1.jar"
        )
        download_and_check(
            "${BASE_URL}org/ejml/ejml-fdense/0.43.1/ejml-fdense-0.43.1.jar"
            "${JAR_ROOT}/ejml-fdense-0.43.1.jar"
        )
        download_and_check(
            "${BASE_URL}org/ejml/ejml-simple/0.43.1/ejml-simple-0.43.1.jar"
            "${JAR_ROOT}/ejml-simple-0.43.1.jar"
        )
        download_and_check(
            "${BASE_URL}org/ejml/ejml-zdense/0.43.1/ejml-zdense-0.43.1.jar"
            "${JAR_ROOT}/ejml-zdense-0.43.1.jar"
        )

        message(STATUS "All files downloaded.")
    endif()

    file(GLOB EJML_JARS "${WPILIB_BINARY_DIR}/wpimath/thirdparty/ejml/*.jar")
    file(GLOB JACKSON_JARS "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson/*.jar")
    file(GLOB QUICKBUF_JAR ${WPILIB_BINARY_DIR}/wpiutil/thirdparty/quickbuf/*.jar)

    set(CMAKE_JNI_TARGET true)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java src/generated/main/java/*.java)

    add_jar(
        wpimath_jar
        ${JAVA_SOURCES}
        INCLUDE_JARS wpiutil_jar wpiunits_jar ${EJML_JARS} ${JACKSON_JARS} ${QUICKBUF_JAR}
        OUTPUT_NAME wpimath
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
        GENERATE_NATIVE_HEADERS wpimath_jni_headers
    )
    set_property(TARGET wpimath_jar PROPERTY FOLDER "java")

    install_jar(wpimath_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS wpimath_jar FILE wpimath_jar.cmake DESTINATION share/wpimath)

    add_library(wpimathjni ${wpimath_jni_src})
    wpilib_target_warnings(wpimathjni)
    target_link_libraries(wpimathjni PUBLIC wpimath)

    set_property(TARGET wpimathjni PROPERTY FOLDER "libraries")

    target_link_libraries(wpimathjni PRIVATE wpimath_jni_headers)
    add_dependencies(wpimathjni wpimath_jar)

    install(TARGETS wpimathjni EXPORT wpimathjni)
    export(TARGETS wpimathjni FILE wpimathjni.cmake NAMESPACE wpimathjni::)
endif()

if(WITH_JAVA_SOURCE)
    include(UseJava)
    include(CreateSourceJar)
    add_source_jar(
        wpimath_src_jar
        BASE_DIRECTORIES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java
            ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/java
        OUTPUT_NAME wpimath-sources
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
    )
    set_property(TARGET wpimath_src_jar PROPERTY FOLDER "java")

    install_jar(wpimath_src_jar DESTINATION ${java_lib_dest})
endif()

file(
    GLOB_RECURSE wpimath_native_src
    src/main/native/cpp/*.cpp
    src/main/native/thirdparty/sleipnir/src/*.cpp
)
list(REMOVE_ITEM wpimath_native_src ${wpimath_jni_src})

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS FALSE)

file(GLOB_RECURSE wpimath_protobuf_native_src src/generated/main/native/cpp/wpimath/protobuf/*.cpp)

add_library(protobuf OBJECT ${wpimath_protobuf_native_src})
target_link_libraries(protobuf wpiutil)

add_library(wpimath ${wpimath_native_src} $<TARGET_OBJECTS:protobuf>)

if(MSVC)
    get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    set(CONFIG_SUFFIX "$<$<BOOL:${IS_MULTI_CONFIG}>:_$<CONFIG>>")
    file(
        GENERATE OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/protobuf_objects${CONFIG_SUFFIX}.txt
        CONTENT $<LIST:JOIN,$<TARGET_OBJECTS:protobuf>,\n>
    )
    add_custom_command(
        TARGET wpimath
        PRE_LINK
        COMMAND
            cmake -E __create_def ${CMAKE_CURRENT_BINARY_DIR}/protobuf_exports${CONFIG_SUFFIX}.def
            ${CMAKE_CURRENT_BINARY_DIR}/protobuf_objects${CONFIG_SUFFIX}.txt
    )
    target_link_options(
        wpimath
        PRIVATE
            /DEF:$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/protobuf_exports${CONFIG_SUFFIX}.def>
    )
endif()

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set_target_properties(wpimath PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET wpimath PROPERTY FOLDER "libraries")
target_compile_definitions(wpimath PRIVATE WPILIB_EXPORTS SLEIPNIR_EXPORTS)

target_compile_features(wpimath PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(wpimath PUBLIC /utf-8 /bigobj)
endif()
wpilib_target_warnings(wpimath)
target_link_libraries(wpimath wpiutil)

if(NOT USE_SYSTEM_EIGEN)
    install(
        DIRECTORY src/main/native/thirdparty/eigen/include/
        DESTINATION "${include_dest}/wpimath"
    )
    target_include_directories(
        wpimath
        SYSTEM
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/eigen/include>
    )
else()
    find_package(Eigen3 CONFIG REQUIRED)
    target_link_libraries(wpimath Eigen3::Eigen)
endif()

install(
    DIRECTORY src/main/native/thirdparty/gcem/include/ src/main/native/thirdparty/sleipnir/include/
    DESTINATION "${include_dest}/wpimath"
)
target_include_directories(
    wpimath
    SYSTEM
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/gcem/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/sleipnir/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/sleipnir/src>
)

install(
    DIRECTORY src/generated/main/native/cpp/
    DESTINATION "${include_dest}/wpimath"
    FILES_MATCHING
    PATTERN "*.h"
)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/wpimath")
target_include_directories(
    wpimath
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/native/cpp>
        $<INSTALL_INTERFACE:${include_dest}/wpimath>
)

install(TARGETS wpimath EXPORT wpimath)
export(TARGETS wpimath FILE wpimath.cmake NAMESPACE wpimath::)

configure_file(wpimath-config.cmake.in ${WPILIB_BINARY_DIR}/wpimath-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/wpimath-config.cmake DESTINATION share/wpimath)
install(EXPORT wpimath DESTINATION share/wpimath)

if(WITH_TESTS)
    wpilib_add_test(wpimath src/test/native/cpp)
    target_include_directories(wpimath_test PRIVATE src/test/native/include)
    target_link_libraries(wpimath_test wpimath googletest)
endif()

project(wpimath)

include(SubDirList)
include(CompileWarnings)
include(AddTest)
include(DownloadAndCheck)

# workaround for makefiles - for some reason parent directories aren't created.
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/protobuf")
file(GLOB wpimath_proto_src src/main/proto/*.proto)
protobuf_generate_cpp(
    WPIMATH_PROTO_SRCS
    WPIMATH_PROTO_HDRS
    PROTOC_OUT_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/protobuf"
    PROTOS
    ${wpimath_proto_src}
)

file(
    GLOB wpimath_jni_src
    src/main/native/cpp/jni/WPIMathJNI_ArmFeedforward.cpp
    src/main/native/cpp/jni/WPIMathJNI_DARE.cpp
    src/main/native/cpp/jni/WPIMathJNI_Eigen.cpp
    src/main/native/cpp/jni/WPIMathJNI_Exceptions.cpp
    src/main/native/cpp/jni/WPIMathJNI_Pose3d.cpp
    src/main/native/cpp/jni/WPIMathJNI_StateSpaceUtil.cpp
    src/main/native/cpp/jni/WPIMathJNI_Trajectory.cpp
)

# Java bindings
if(WITH_JAVA)
    find_package(Java REQUIRED)
    find_package(JNI REQUIRED)
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

    set(CMAKE_JAVA_INCLUDE_PATH wpimath.jar ${EJML_JARS} ${JACKSON_JARS} ${QUICKBUF_JAR})

    set(CMAKE_JNI_TARGET true)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java src/generated/main/java/*.java)

    add_jar(
        wpimath_jar
        ${JAVA_SOURCES}
        INCLUDE_JARS ${EJML_JARS} wpiutil_jar wpiunits_jar
        OUTPUT_NAME wpimath
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
    find_package(Java REQUIRED)
    include(UseJava)
    file(
        GLOB WPIMATH_SOURCES
        src/main/java/edu/wpi/first/math/*.java
        src/generated/main/java/edu/wpi/first/math/Nat.java
    )
    file(GLOB WPIMATH_CONTROLLER_SOURCES src/main/java/edu/wpi/first/math/controller/*.java)
    file(GLOB WPIMATH_ESTIMATOR_SOURCES src/main/java/edu/wpi/first/math/estimator/*.java)
    file(GLOB WPIMATH_FILTER_SOURCES src/main/java/edu/wpi/first/math/filter/*.java)
    file(GLOB WPIMATH_GEOMETRY_SOURCES src/main/java/edu/wpi/first/math/geometry/*.java)
    file(GLOB WPIMATH_INTERPOLATION_SOURCES src/main/java/edu/wpi/first/math/interpolation/*.java)
    file(GLOB WPIMATH_KINEMATICS_SOURCES src/main/java/edu/wpi/first/math/kinematics/*.java)
    file(GLOB WPIMATH_NUMBERS_SOURCES src/generated/main/java/edu/wpi/first/math/numbers/*.java)
    file(GLOB WPIMATH_SPLINE_SOURCES src/main/java/edu/wpi/first/math/spline/*.java)
    file(GLOB WPIMATH_SYSTEM_SOURCES src/main/java/edu/wpi/first/math/system/*.java)
    file(GLOB WPIMATH_SYSTEM_PLANT_SOURCES src/main/java/edu/wpi/first/math/system/plant/*.java)
    file(GLOB WPIMATH_TRAJECTORY_SOURCES src/main/java/edu/wpi/first/math/trajectory/*.java)
    file(
        GLOB WPIMATH_TRAJECTORY_CONSTRAINT_SOURCES
        src/main/java/edu/wpi/first/math/trajectory/constraint/*.java
    )
    add_jar(
        wpimath_src_jar
        RESOURCES
        NAMESPACE "edu/wpi/first/math" ${WPIMATH_SOURCES}
        NAMESPACE "edu/wpi/first/math/controller" ${WPIMATH_CONTROLLER_SOURCES}
        NAMESPACE "edu/wpi/first/math/estimator" ${WPIMATH_ESTIMATOR_SOURCES}
        NAMESPACE "edu/wpi/first/math/filter" ${WPIMATH_FILTER_SOURCES}
        NAMESPACE "edu/wpi/first/math/geometry" ${WPIMATH_GEOMETRY_SOURCES}
        NAMESPACE "edu/wpi/first/math/interpolation" ${WPIMATH_INTERPOLATION_SOURCES}
        NAMESPACE "edu/wpi/first/math/kinematics" ${WPIMATH_KINEMATICS_SOURCES}
        NAMESPACE "edu/wpi/first/math/spline" ${WPIMATH_SPLINE_SOURCES}
        NAMESPACE "edu/wpi/first/math/system" ${WPIMATH_SYSTEM_SOURCES}
        NAMESPACE "edu/wpi/first/math/system/plant" ${WPIMATH_SYSTEM_PLANT_SOURCES}
        NAMESPACE "edu/wpi/first/math/trajectory" ${WPIMATH_TRAJECTORY_SOURCES}
        NAMESPACE
            "edu/wpi/first/math/trajectory/constraint"
            ${WPIMATH_TRAJECTORY_CONSTRAINT_SOURCES}
        NAMESPACE "edu/wpi/first/math/util" src/main/java/edu/wpi/first/math/util/Units.java
        OUTPUT_NAME wpimath-sources
    )

    get_property(WPIMATH_SRC_JAR_FILE TARGET wpimath_src_jar PROPERTY JAR_FILE)
    install(FILES ${WPIMATH_SRC_JAR_FILE} DESTINATION "${java_lib_dest}")

    set_property(TARGET wpimath_src_jar PROPERTY FOLDER "java")
endif()

file(
    GLOB_RECURSE wpimath_native_src
    src/main/native/cpp/*.cpp
    src/main/native/thirdparty/sleipnir/src/*.cpp
)
list(REMOVE_ITEM wpimath_native_src ${wpimath_jni_src})

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS FALSE)
add_library(wpimath ${wpimath_native_src} ${WPIMATH_PROTO_SRCS})
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
            $<INSTALL_INTERFACE:${include_dest}/wpimath>
    )
else()
    find_package(Eigen3 CONFIG REQUIRED)
    target_link_libraries(wpimath Eigen3::Eigen)
endif()

install(
    DIRECTORY src/main/native/thirdparty/sleipnir/include/
    DESTINATION "${include_dest}/wpimath"
)
target_include_directories(
    wpimath
    SYSTEM
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/sleipnir/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/sleipnir/src>
)

install(DIRECTORY src/main/native/thirdparty/gcem/include/ DESTINATION "${include_dest}/wpimath")
target_include_directories(
    wpimath
    SYSTEM
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/gcem/include>
)

install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/wpimath")
target_include_directories(
    wpimath
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/protobuf>
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
    target_link_libraries(wpimath_test wpimath gmock_main)
endif()

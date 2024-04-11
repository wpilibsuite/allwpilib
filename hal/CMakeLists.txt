project(hal)

include(CompileWarnings)
include(AddTest)

file(
    GLOB hal_shared_native_src
    src/main/native/cpp/*.cpp
    hal_shared_native_src
    src/main/native/cpp/cpp/*.cpp
    hal_shared_native_src
    src/main/native/cpp/handles/*.cpp
    hal_sim_native_src
    src/main/native/sim/*.cpp
    hal_sim_native_src
    src/main/native/sim/mockdata/*.cpp
)
add_library(hal ${hal_shared_native_src})
wpilib_target_warnings(hal)
set_target_properties(hal PROPERTIES DEBUG_POSTFIX "d")

if(USE_EXTERNAL_HAL)
    include(${EXTERNAL_HAL_FILE})
else()
    target_sources(hal PRIVATE ${hal_sim_native_src})
endif()

set_target_properties(hal PROPERTIES OUTPUT_NAME "wpiHal")

target_include_directories(
    hal
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/hal>
)

target_include_directories(
    hal
    PUBLIC
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
    find_package(Java REQUIRED)
    find_package(JNI REQUIRED)
    include(UseJava)
    set(CMAKE_JAVA_COMPILE_FLAGS "-encoding" "UTF8" "-Xlint:unchecked")

    configure_file(src/generate/FRCNetComm.java.in FRCNetComm.java)

    file(GLOB_RECURSE hal_shared_jni_src src/main/native/cpp/jni/*.cpp)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java src/generated/main/java/*.java)
    set(CMAKE_JNI_TARGET true)

    add_jar(
        hal_jar
        ${JAVA_SOURCES}
        INCLUDE_JARS wpiutil_jar
        OUTPUT_NAME wpiHal
        GENERATE_NATIVE_HEADERS hal_jni_headers
    )

    install_jar(hal_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS hal_jar FILE hal_jar.cmake DESTINATION share/hal)

    add_library(haljni ${hal_shared_jni_src})

    if(USE_EXTERNAL_HAL)
        include(${EXTERNAL_HAL_FILE})
    endif()

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
    find_package(Java REQUIRED)
    include(UseJava)
    file(GLOB HAL_SOURCES src/main/java/edu/wpi/first/hal/*.java src/generated/main/java/*.java)
    file(GLOB HAL_CAN_SOURCES src/main/java/edu/wpi/first/hal/can/*.java)
    file(GLOB HAL_SIMULATION_SOURCES src/main/java/edu/wpi/first/hal/simulation/*.java)
    file(GLOB HAL_UTIL_SOURCES src/main/java/edu/wpi/first/hal/util/*.java)
    add_jar(
        hal_src_jar
        RESOURCES
        NAMESPACE "edu/wpi/first/hal" ${HAL_SOURCES}
        NAMESPACE "edu/wpi/first/hal/can" ${HAL_CAN_SOURCES}
        NAMESPACE
            "edu/wpi/first/hal/communication"
            src/main/java/edu/wpi/first/hal/communication/NIRioStatus.java
        NAMESPACE "edu/wpi/first/hal/simulation" ${HAL_SIMULATION_SOURCES}
        NAMESPACE "edu/wpi/first/hal/util" ${HAL_UTIL_SOURCES}
        OUTPUT_NAME wpiHal-sources
    )

    get_property(HAL_SRC_JAR_FILE TARGET hal_src_jar PROPERTY JAR_FILE)
    install(FILES ${HAL_SRC_JAR_FILE} DESTINATION "${java_lib_dest}")

    set_property(TARGET hal_src_jar PROPERTY FOLDER "java")
endif()

if(WITH_TESTS)
    wpilib_add_test(hal src/test/native/cpp)
    target_link_libraries(hal_test hal gtest)
endif()

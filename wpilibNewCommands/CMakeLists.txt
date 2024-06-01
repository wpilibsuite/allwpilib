project(wpilibNewCommands)

include(SubDirList)
include(CompileWarnings)
include(AddTest)

if(WITH_JAVA)
    find_package(Java REQUIRED)
    include(UseJava)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
    add_jar(
        wpilibNewCommands_jar
        ${JAVA_SOURCES}
        INCLUDE_JARS
            hal_jar
            ntcore_jar
            cscore_jar
            cameraserver_jar
            wpimath_jar
            wpiunits_jar
            wpiutil_jar
            wpilibj_jar
        OUTPUT_NAME wpilibNewCommands
    )
    set_property(TARGET wpilibNewCommands_jar PROPERTY FOLDER "java")

    install_jar(wpilibNewCommands_jar DESTINATION ${java_lib_dest})
    install_jar_exports(
        TARGETS wpilibNewCommands_jar
        FILE wpilibNewCommands_jar.cmake
        DESTINATION share/wpilibNewCommands
    )
endif()

if(WITH_JAVA_SOURCE)
    find_package(Java REQUIRED)
    include(UseJava)
    file(GLOB WPILIBNEWCOMMANDS_SOURCES src/main/java/edu/wpi/first/wpilibj2/command/*.java)
    file(
        GLOB WPILIBNEWCOMMANDS_BUTTON_SOURCES
        src/main/java/edu/wpi/first/wpilibj2/command/button*.java
    )
    add_jar(
        wpilibNewCommands_src_jar
        RESOURCES
        NAMESPACE "edu/wpi/first/wpilibj2/command" ${WPILIBNEWCOMMANDS_SOURCES}
        NAMESPACE "edu/wpi/first/wpilibj2/command/button" ${WPILIBNEWCOMMANDS_BUTTON_SOURCES}
        OUTPUT_NAME wpilibNewCommands-sources
    )

    get_property(WPILIBNEWCOMMANDS_SRC_JAR_FILE TARGET wpilibNewCommands_src_jar PROPERTY JAR_FILE)
    install(FILES ${WPILIBNEWCOMMANDS_SRC_JAR_FILE} DESTINATION "${java_lib_dest}")

    set_property(TARGET wpilibNewCommands_src_jar PROPERTY FOLDER "java")
endif()

file(GLOB_RECURSE wpilibNewCommands_native_src src/main/native/cpp/*.cpp)
add_library(wpilibNewCommands ${wpilibNewCommands_native_src})
set_target_properties(wpilibNewCommands PROPERTIES DEBUG_POSTFIX "d")
set_property(TARGET wpilibNewCommands PROPERTY FOLDER "libraries")

target_compile_features(wpilibNewCommands PUBLIC cxx_std_20)
wpilib_target_warnings(wpilibNewCommands)
target_link_libraries(wpilibNewCommands wpilibc)

target_include_directories(
    wpilibNewCommands
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/wpilibNewCommands>
)

install(TARGETS wpilibNewCommands EXPORT wpilibnewcommands)
export(TARGETS wpilibNewCommands FILE wpilibnewcommands.cmake NAMESPACE wpilibnewcommands::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/wpilibNewCommands")

configure_file(
    wpilibnewcommands-config.cmake.in
    ${WPILIB_BINARY_DIR}/wpilibnewcommands-config.cmake
)
install(
    FILES ${WPILIB_BINARY_DIR}/wpilibnewcommands-config.cmake
    DESTINATION share/wpilibNewCommands
)
install(EXPORT wpilibnewcommands DESTINATION share/wpilibNewCommands)

if(WITH_TESTS)
    wpilib_add_test(wpilibNewCommands src/test/native/cpp)
    target_include_directories(wpilibNewCommands_test PRIVATE src/test/native/include)
    target_link_libraries(wpilibNewCommands_test wpilibNewCommands gmock_main)
endif()

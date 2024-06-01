project(romiVendordep)

include(SubDirList)
include(CompileWarnings)
include(AddTest)

if(WITH_JAVA)
    find_package(Java REQUIRED)
    include(UseJava)

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
    add_jar(
        romiVendordep_jar
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
        OUTPUT_NAME romiVendordep
    )
    set_property(TARGET romiVendordep_jar PROPERTY FOLDER "java")

    install_jar(romiVendordep_jar DESTINATION ${java_lib_dest})
    install_jar_exports(
        TARGETS romiVendordep_jar
        FILE romiVendordep_jar.cmake
        DESTINATION share/romiVendordep
    )
endif()

if(WITH_JAVA_SOURCE)
    find_package(Java REQUIRED)
    include(UseJava)
    file(GLOB_RECURSE ROMIVENDORDEP_SOURCES src/main/java/*.java)
    add_jar(
        romiVendordep_src_jar
        RESOURCES
        NAMESPACE "edu/wpi/first/wpilibj/romi" ${ROMIVENDORDEP_SOURCES}
        OUTPUT_NAME romiVendordep-sources
    )

    get_property(ROMIVENDORDEP_SRC_JAR_FILE TARGET romiVendordep_src_jar PROPERTY JAR_FILE)
    install(FILES ${ROMIVENDORDEP_JAR_FILE} DESTINATION "${java_lib_dest}")

    set_property(TARGET romiVendordep_src_jar PROPERTY FOLDER "java")
endif()

file(GLOB_RECURSE romiVendordep_native_src src/main/native/cpp/*.cpp)
add_library(romiVendordep ${romiVendordep_native_src})
set_target_properties(romiVendordep PROPERTIES DEBUG_POSTFIX "d")
set_property(TARGET romiVendordep PROPERTY FOLDER "libraries")

target_compile_features(romiVendordep PUBLIC cxx_std_20)
wpilib_target_warnings(romiVendordep)
target_link_libraries(romiVendordep wpilibc)

target_include_directories(
    romiVendordep
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/romiVendordep>
)

install(TARGETS romiVendordep EXPORT romivendordep DESTINATION "${main_lib_dest}")
export(TARGETS romiVendordep FILE romivendordep.cmake NAMESPACE romivendordep::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/romiVendordep")

configure_file(romivendordep-config.cmake.in ${WPILIB_BINARY_DIR}/romivendordep-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/romivendordep-config.cmake DESTINATION share/romiVendordep)
install(EXPORT romivendordep DESTINATION share/romiVendordep)

if(WITH_TESTS)
    wpilib_add_test(romiVendordep src/test/native/cpp)
    target_include_directories(romiVendordep_test PRIVATE src/test/native/include)
    target_link_libraries(romiVendordep_test romiVendordep gmock_main)
endif()

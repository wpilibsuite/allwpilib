project(fields)

include(CompileWarnings)
include(GenResources)

if(WITH_JAVA)
    include(UseJava)

    file(GLOB JACKSON_JARS "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson/*.jar")

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
    file(
        GLOB_RECURSE JAVA_RESOURCES
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        src/main/native/resources/*.json
        src/main/native/resources/*.png
        src/main/native/resources/*.jpg
    )
    add_jar(
        field_images_jar
        SOURCES ${JAVA_SOURCES}
        RESOURCES
        NAMESPACE "org/wpilib/fields" ${JAVA_RESOURCES}
        INCLUDE_JARS ${JACKSON_JARS}
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
        OUTPUT_NAME fields
    )
    set_property(TARGET field_images_jar PROPERTY FOLDER "java")
    install_jar(field_images_jar DESTINATION ${java_lib_dest})
    install_jar_exports(TARGETS field_images_jar FILE fields_jar.cmake DESTINATION share/fields)
endif()

generate_resources(
    src/main/native/resources/org/wpilib/fields
    ${CMAKE_CURRENT_BINARY_DIR}/generated/main/cpp
    FIELDS
    wpi::fields
    field_images_resources_src
)

add_library(fields ${field_images_resources_src} src/main/native/cpp/fields.cpp)
set_target_properties(fields PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET fields PROPERTY FOLDER "libraries")
target_compile_features(fields PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(fields PUBLIC /bigobj)
endif()
wpilib_target_warnings(fields)

install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/fields")
target_include_directories(
    fields
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/fields>
)

install(TARGETS fields EXPORT fields)
export(TARGETS fields FILE fields.cmake NAMESPACE fields::)

configure_file(fields-config.cmake.in ${WPILIB_BINARY_DIR}/fields-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/fields-config.cmake DESTINATION share/fields)
install(EXPORT fields DESTINATION share/fields)

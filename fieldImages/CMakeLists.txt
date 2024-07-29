project(fieldImages)

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
        NAMESPACE "edu/wpi/first/fields" ${JAVA_RESOURCES}
        INCLUDE_JARS ${JACKSON_JARS}
        OUTPUT_DIR ${WPILIB_BINARY_DIR}/${java_lib_dest}
        OUTPUT_NAME fieldImages
    )
    set_property(TARGET field_images_jar PROPERTY FOLDER "java")
    install_jar(field_images_jar DESTINATION ${java_lib_dest})
    install_jar_exports(
        TARGETS field_images_jar
        FILE fieldImages_jar.cmake
        DESTINATION share/fieldImages
    )
endif()

generate_resources(
    src/main/native/resources/edu/wpi/first/fields
    generated/main/cpp
    FIELDS
    fields
    field_images_resources_src
)

add_library(fieldImages src/main/native/cpp/fields.cpp ${field_images_resources_src})
set_target_properties(fieldImages PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET fieldImages PROPERTY FOLDER "libraries")
target_compile_features(fieldImages PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(fieldImages PUBLIC /bigobj)
endif()
wpilib_target_warnings(fieldImages)

install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/fieldImages")
target_include_directories(
    fieldImages
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/fieldImages>
)

install(TARGETS fieldImages EXPORT fieldimages)
export(TARGETS fieldImages FILE fieldimages.cmake NAMESPACE fieldimages::)

configure_file(fieldimages-config.cmake.in ${WPILIB_BINARY_DIR}/fieldimages-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/fieldimages-config.cmake DESTINATION share/fieldImages)
install(EXPORT fieldimages DESTINATION share/fieldImages)

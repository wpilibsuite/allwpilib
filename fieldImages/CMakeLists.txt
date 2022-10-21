project(fieldImages)

include(CompileWarnings)
include(GenResources)

if (WITH_JAVA)
    find_package(Java REQUIRED)
    include(UseJava)

    file(GLOB JACKSON_JARS "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson/*.jar")
    set(CMAKE_JAVA_INCLUDE_PATH fieldImages.jar ${JACKSON_JARS})

    file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
    file(GLOB_RECURSE JAVA_RESOURCES src/main/native/resources/*.json src/main/native/resources/*.png src/main/native/resources/*.jpg)
    add_jar(field_images_jar SOURCES ${JAVA_SOURCES} RESOURCES NAMESPACE "edu/wpi/first/fields" ${JAVA_RESOURCES} OUTPUT_NAME fieldImages)

    get_property(FIELD_IMAGES_JAR_FILE TARGET field_images_jar PROPERTY JAR_FILE)
    install(FILES ${FIELD_IMAGES_JAR_FILE} DESTINATION "${java_lib_dest}")

    set_property(TARGET field_images_jar PROPERTY FOLDER "java")

endif()


GENERATE_RESOURCES(src/main/native/resources/edu/wpi/first/fields generated/main/cpp FIELDS fields field_images_resources_src)


add_library(fieldImages ${field_images_resources_src})
set_target_properties(fieldImages PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET fieldImages PROPERTY FOLDER "libraries")
target_compile_features(fieldImages PUBLIC cxx_std_20)
if (MSVC)
    target_compile_options(fieldImages PUBLIC /bigobj)
endif()
wpilib_target_warnings(fieldImages)

target_include_directories(fieldImages PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
                            $<INSTALL_INTERFACE:${include_dest}/fields>)

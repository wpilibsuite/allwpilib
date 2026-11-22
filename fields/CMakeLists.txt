project(fields)

include(CompileWarnings)
include(GenResources)

generate_resources(
    src/main/native/resources/org/wpilib/fields
    ${CMAKE_CURRENT_BINARY_DIR}/generated/main/cpp
    FIELDS
    wpi::fields
    field_resources_src
    *.json
)

set(fields_export_targets fields)

if(WPILIB_WITH_GUI)
    generate_resources(
        src/main/native/resources/org/wpilib/fields
        ${CMAKE_CURRENT_BINARY_DIR}/generated/mainImages/cpp
        FIELD_IMAGES
        wpi::fields
        field_images_resources_src
        *.jpg
        *.jpeg
        *.png
    )
endif()

add_library(
    fields
    ${field_resources_src}
    src/generated/main/native/cpp/fields/fields.cpp
    src/main/native/cpp/FieldTag.cpp
    src/main/native/cpp/FieldImage.cpp
    src/main/native/cpp/Field.cpp
)
set_target_properties(fields PROPERTIES DEBUG_POSTFIX "d")

if(WPILIB_WITH_GUI)
    add_library(
        fieldimages
        ${field_images_resources_src}
        src/generated/main/native/cpp/fieldimages/field_images.cpp
    )
    set_target_properties(fieldimages PROPERTIES DEBUG_POSTFIX "d")
    list(APPEND fields_export_targets fieldimages)
endif()

set_property(TARGET fields PROPERTY FOLDER "libraries")
target_compile_features(fields PUBLIC cxx_std_23)
target_link_libraries(fields PUBLIC wpimath wpiutil)
if(MSVC)
    target_compile_options(fields PUBLIC /bigobj)
endif()
wpilib_target_warnings(fields)

if(WPILIB_WITH_GUI)
    set_property(TARGET fieldimages PROPERTY FOLDER "libraries")
    target_compile_features(fieldimages PUBLIC cxx_std_23)
    target_link_libraries(fieldimages PUBLIC wpiutil)
    target_include_directories(
        fieldimages
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/native/include>
            $<INSTALL_INTERFACE:${include_dest}/fields>
    )
    if(MSVC)
        target_compile_options(fieldimages PUBLIC /bigobj)
    endif()
    wpilib_target_warnings(fieldimages)
endif()

install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/fields")
install(DIRECTORY src/generated/main/native/include/ DESTINATION "${include_dest}/fields")
target_include_directories(
    fields
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/fields>
)

install(TARGETS ${fields_export_targets} EXPORT fields)
export(TARGETS ${fields_export_targets} FILE fields.cmake NAMESPACE fields::)

configure_file(fields-config.cmake.in ${WPILIB_BINARY_DIR}/fields-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/fields-config.cmake DESTINATION share/fields)
install(EXPORT fields DESTINATION share/fields)

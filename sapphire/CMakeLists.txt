project(sapphire)

include(CompileWarnings)
include(GenResources)
include(LinkMacOSGUI)

configure_file(src/main/generate/WPILibVersion.cpp.in WPILibVersion.cpp)
GENERATE_RESOURCES(src/main/native/resources generated/main/cpp DLT dlt sapphire_resources_src)

file(GLOB sapphire_src src/main/native/cpp/*.cpp ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.cpp)

if (WIN32)
    set(sapphire_rc src/main/native/win/sapphire.rc)
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE sapphire.icns)
    set(APP_ICON_MACOSX src/main/native/mac/sapphire.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

add_executable(sapphire ${sapphire_src} ${sapphire_resources_src} ${sapphire_rc} ${APP_ICON_MACOSX})
wpilib_link_macos_gui(sapphire)
target_link_libraries(sapphire libglass ${LIBSSH_LIBRARIES})
target_include_directories(sapphire PRIVATE ${LIBSSH_INCLUDE_DIRS})

if (WIN32)
    set_target_properties(sapphire PROPERTIES WIN32_EXECUTABLE YES)
elseif(APPLE)
    set_target_properties(sapphire PROPERTIES MACOSX_BUNDLE YES OUTPUT_NAME "sapphire")
endif()

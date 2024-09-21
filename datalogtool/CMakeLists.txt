project(datalogtool)

include(CompileWarnings)
include(GenResources)
include(LinkMacOSGUI)

configure_file(src/main/generate/WPILibVersion.cpp.in WPILibVersion.cpp)
generate_resources(src/main/native/resources generated/main/cpp DLT dlt datalogtool_resources_src)

file(GLOB datalogtool_src src/main/native/cpp/*.cpp ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.cpp)

if(WIN32)
    set(datalogtool_rc src/main/native/win/datalogtool.rc)
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE datalogtool.icns)
    set(APP_ICON_MACOSX src/main/native/mac/datalogtool.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

add_executable(
    datalogtool
    ${datalogtool_src}
    ${datalogtool_resources_src}
    ${datalogtool_rc}
    ${APP_ICON_MACOSX}
)
wpilib_link_macos_gui(datalogtool)
target_link_libraries(datalogtool libglass ssh)

if(WIN32)
    set_target_properties(datalogtool PROPERTIES WIN32_EXECUTABLE YES)
elseif(APPLE)
    set_target_properties(datalogtool PROPERTIES MACOSX_BUNDLE YES OUTPUT_NAME "datalogTool")
endif()

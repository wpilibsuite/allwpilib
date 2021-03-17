project(outlineviewer)

include(CompileWarnings)
include(GenResources)
include(LinkMacOSGUI)

configure_file(src/main/generate/WPILibVersion.cpp.in WPILibVersion.cpp)
GENERATE_RESOURCES(src/main/native/resources generated/main/cpp OV ov outlineviewer_resources_src)

file(GLOB outlineviewer_src src/main/native/cpp/*.cpp ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.cpp)

if (WIN32)
    set(outlineviewer_rc src/main/native/win/outlineviewer.rc)
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE ov.icns)
    set(APP_ICON_MACOSX src/main/native/mac/ov.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

add_executable(outlineviewer ${outlineviewer_src} ${outlineviewer_resources_src} ${outlineviewer_rc} ${APP_ICON_MACOSX})
wpilib_link_macos_gui(outlineviewer)
target_link_libraries(outlineviewer libglassnt libglass)

if (WIN32)
    set_target_properties(outlineviewer PROPERTIES WIN32_EXECUTABLE YES)
elseif(APPLE)
    set_target_properties(outlineviewer PROPERTIES MACOSX_BUNDLE YES OUTPUT_NAME "OutlineViewer")
endif()

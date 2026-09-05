project(outlineviewer)

include(CompileWarnings)
include(AddTest)
include(GenResources)
include(LinkMacOSGUI)

configure_file(src/main/generate/WPILibVersion.cpp.in WPILibVersion.cpp)
generate_resources(src/main/native/resources generated/main/cpp OV ov outlineviewer_resources_src)

file(GLOB outlineviewer_src src/main/native/cpp/*.cpp ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.cpp)

if(WIN32)
    set(outlineviewer_rc src/main/native/win/outlineviewer.rc)
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE ov.icns)
    set(APP_ICON_MACOSX src/main/native/mac/ov.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

add_executable(
    outlineviewer
    ${outlineviewer_src}
    ${outlineviewer_resources_src}
    ${outlineviewer_rc}
    ${APP_ICON_MACOSX}
)
wpilib_link_macos_gui(outlineviewer)
wpilib_target_warnings(outlineviewer)
target_link_libraries(outlineviewer libglassnt libglass)

if(WIN32)
    set_target_properties(outlineviewer PROPERTIES WIN32_EXECUTABLE YES)
elseif(APPLE)
    set_target_properties(outlineviewer PROPERTIES MACOSX_BUNDLE YES OUTPUT_NAME "OutlineViewer")
endif()

if(WPILIB_WITH_IMGUI_TESTS)
    wpilib_add_imgui_test(outlineviewer src/test/native/imgui)
    wpilib_link_macos_gui(outlineviewer_imgui_test)
    target_sources(
        outlineviewer_imgui_test
        PRIVATE
            ${outlineviewer_src}
            ${outlineviewer_resources_src}
            ${outlineviewer_rc}
            ${APP_ICON_MACOSX}
    )
    target_link_libraries(outlineviewer_imgui_test libglassnt_imgui_test libglass_imgui_test)
endif()

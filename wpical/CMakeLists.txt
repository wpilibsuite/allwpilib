project(wpical)

include(CompileWarnings)
include(GenResources)
include(LinkMacOSGUI)

configure_file(src/main/generate/WPILibVersion.cpp.in WPILibVersion.cpp)
generate_resources(src/main/native/resources generated/main/cpp OV ov wpical_resources_src)

file(GLOB wpical_src src/main/native/cpp/*.cpp ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.cpp)

if(WIN32)
    set(wpical_rc src/main/native/win/wpical.rc)
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE ov.icns)
    set(APP_ICON_MACOSX src/main/native/mac/ov.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

add_executable(wpical ${wpical_src} ${wpical_resources_src} ${wpical_rc} ${APP_ICON_MACOSX})
wpilib_link_macos_gui(wpical)
wpilib_target_warnings(wpical)
target_link_libraries(wpical libglassnt libglass)

if(WIN32)
    set_target_properties(wpical PROPERTIES WIN32_EXECUTABLE YES)
elseif(APPLE)
    set_target_properties(wpical PROPERTIES MACOSX_BUNDLE YES OUTPUT_NAME "wpical")
endif()

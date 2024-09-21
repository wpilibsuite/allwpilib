project(roborioteamnumbersetter)

include(CompileWarnings)
include(GenResources)
include(LinkMacOSGUI)

configure_file(src/main/generate/WPILibVersion.cpp.in WPILibVersion.cpp)
generate_resources(src/main/native/resources generated/main/cpp RTNS rtns rtns_resources_src)

file(GLOB rtns_src src/main/native/cpp/*.cpp ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.cpp)

if(WIN32)
    set(rtns_rc src/main/native/win/roborioteamnumbersetter.rc)
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE rtns.icns)
    set(APP_ICON_MACOSX src/main/native/mac/rtns.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

add_executable(
    roborioteamnumbersetter
    ${rtns_src}
    ${rtns_resources_src}
    ${rtns_rc}
    ${APP_ICON_MACOSX}
)
wpilib_link_macos_gui(roborioteamnumbersetter)
target_link_libraries(roborioteamnumbersetter libglass wpinet ssh)

if(WIN32)
    set_target_properties(roborioteamnumbersetter PROPERTIES WIN32_EXECUTABLE YES)
elseif(APPLE)
    set_target_properties(
        roborioteamnumbersetter
        PROPERTIES MACOSX_BUNDLE YES OUTPUT_NAME "roborioTeamNumberSetter"
    )
endif()

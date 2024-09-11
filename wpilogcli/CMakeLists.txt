project(wpilogcli)

include(CompileWarnings)
include(GenResources)

configure_file(src/main/generate/WPILibVersion.cpp.in WPILibVersion.cpp)
generate_resources(src/main/native/resources generated/main/cpp DLT dlt wpilogcli_resources_src)

file(GLOB wpilogcli_src src/main/native/cpp/*.cpp ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.cpp)

if(WIN32)
    set(wpilogcli_rc src/main/native/win/wpilogcli.rc)
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE wpilogcli.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

add_executable(
    wpilogcli
    ${wpilogcli_src}
    ${wpilogcli_resources_src}
    ${wpilogcli_rc}
    ${APP_ICON_MACOSX}
)

target_link_libraries(wpilogcli PRIVATE wpiutil)

if(WIN32)
    set_target_properties(wpilogcli PROPERTIES WIN32_EXECUTABLE YES)
elseif(APPLE)
    set_target_properties(wpilogcli PROPERTIES MACOSX_BUNDLE YES OUTPUT_NAME "wpilogcli")
endif()
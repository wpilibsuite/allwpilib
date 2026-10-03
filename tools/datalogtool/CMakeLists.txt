project(datalogtool)

include(CompileWarnings)
include(AddTest)
include(GenResources)
include(LinkMacOSGUI)

configure_file(src/main/generate/WPILibVersion.cpp.in WPILibVersion.cpp)
set(datalogtool_cli_src
    src/main/native/cli/Cli.cpp
    src/main/native/cpp/CsvExport.cpp
    src/main/native/cpp/Sftp.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.cpp
)

add_executable(datalogtool-cli src/main/native/cli/main.cpp ${datalogtool_cli_src})
target_include_directories(datalogtool-cli PRIVATE src/main/native/cpp)
target_link_libraries(datalogtool-cli ssh datalog wpiutil)
wpilib_target_warnings(datalogtool-cli)

if(WPILIB_WITH_TESTS)
    wpilib_add_test(datalogtool src/test/native/cpp)
    target_sources(datalogtool_test PRIVATE ${datalogtool_cli_src})
    target_include_directories(datalogtool_test PRIVATE src/main/native/cpp src/main/native/cli)
    target_link_libraries(datalogtool_test ssh datalog wpiutil)
endif()

if(NOT WPILIB_WITH_GUI)
    return()
endif()

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
target_link_libraries(datalogtool libglass ssh datalog wpiutil)

if(WIN32)
    set_target_properties(datalogtool PROPERTIES WIN32_EXECUTABLE YES)
elseif(APPLE)
    set_target_properties(datalogtool PROPERTIES MACOSX_BUNDLE YES OUTPUT_NAME "datalogTool")
endif()

if(WPILIB_WITH_IMGUI_TESTS)
    wpilib_add_imgui_test(datalogtool src/test/native/imgui)
    wpilib_link_macos_gui(datalogtool_imgui_test)
    target_sources(
        datalogtool_imgui_test
        PRIVATE ${datalogtool_src} ${datalogtool_resources_src} ${datalogtool_rc} ${APP_ICON_MACOSX}
    )
    target_link_libraries(datalogtool_imgui_test libglass_for_tests ssh datalog wpiutil)
endif()

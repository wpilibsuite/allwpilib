project(datalogcli)

include(CompileWarnings)
include(GenResources)

configure_file(src/main/generate/WPILibVersion.cpp.in WPILibVersion.cpp)
generate_resources(src/main/native/resources generated/main/cpp DLT dlt datalogcli_resources_src)

file(GLOB datalogcli_src src/main/native/cpp/*.cpp ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.cpp)

if(WIN32)
    set(datalogcli_rc src/main/native/win/datalogcli.rc)
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE datalogcli.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

add_executable(
    datalogcli
    ${datalogcli_src}
    ${datalogcli_resources_src}
    ${datalogcli_rc}
    ${APP_ICON_MACOSX}
)

target_include_directories(datalogcli PUBLIC src/main/native/include)

target_link_libraries(datalogcli PRIVATE wpiutil)

if(WIN32)
    set_target_properties(datalogcli PROPERTIES WIN32_EXECUTABLE YES)
elseif(APPLE)
    set_target_properties(datalogcli PROPERTIES MACOSX_BUNDLE YES OUTPUT_NAME "datalogcli")
endif()
project(datalog-export)

include(CompileWarnings)
include(GenResources)

configure_file(src/main/generate/WPILibVersion.cpp.in WPILibVersion.cpp)
generate_resources(src/main/native/resources generated/main/cpp DLT dlt datalog-export_resources_src)

file(GLOB datalog-export_src src/main/native/cpp/*.cpp ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.cpp)

if(WIN32)
    set(datalog-export_rc src/main/native/win/datalog-export.rc)
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE datalog-export.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

add_executable(
    datalog-export
    ${datalog-export_src}
    ${datalog-export_resources_src}
    ${datalog-export_rc}
    ${APP_ICON_MACOSX}
)

target_include_directories(datalog-export PUBLIC src/main/native/include)

target_link_libraries(datalog-export PRIVATE wpiutil)

if(WIN32)
    set_target_properties(datalog-export PROPERTIES WIN32_EXECUTABLE YES)
elseif(APPLE)
    set_target_properties(datalog-export PROPERTIES MACOSX_BUNDLE YES OUTPUT_NAME "datalog-export")
endif()
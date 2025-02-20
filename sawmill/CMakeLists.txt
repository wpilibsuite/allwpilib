project(sawmill)

include(CompileWarnings)
include(GenResources)

configure_file(src/main/generate/WPILibVersion.cpp.in WPILibVersion.cpp)
# generate_resources(src/main/native/resources generated/main/cpp DLT dlt sawmill_resources_src)

# Generate compile_commands.json by default
if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_EXPORT_COMPILE_COMMANDS "YES" CACHE STRING "" FORCE)
endif()

file(GLOB sawmill_src src/main/native/cpp/*.cpp ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.cpp)

if(WIN32)
    # set(sawmill_rc src/main/native/win/sawmill.rc)
elseif(APPLE)
    # set(MACOSX_BUNDLE_ICON_FILE sawmill.icns)
    # set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

add_executable(sawmill ${sawmill_src}) #${sawmill_resources_src} ${sawmill_rc} ${APP_ICON_MACOSX})

target_include_directories(sawmill PUBLIC src/main/native/include)

target_link_libraries(sawmill PRIVATE wpiutil datalog)

if(WIN32)
    set_target_properties(sawmill PROPERTIES WIN32_EXECUTABLE YES)
elseif(APPLE)
    set_target_properties(sawmill PROPERTIES MACOSX_BUNDLE YES OUTPUT_NAME "sawmill")
endif()

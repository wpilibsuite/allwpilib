project(glass)

include(CompileWarnings)
include(GenResources)
include(LinkMacOSGUI)

#
# libglass
#
file(GLOB_RECURSE libglass_src src/lib/native/cpp/*.cpp)

add_library(libglass STATIC ${libglass_src})
set_target_properties(libglass PROPERTIES DEBUG_POSTFIX "d")
# Library name can't be glass on Windows or else it will overwrite the Glass app PDB file
if(NOT MSVC)
    set_target_properties(libglass PROPERTIES OUTPUT_NAME "glass")
endif()

set_property(TARGET libglass PROPERTY FOLDER "libraries")

wpilib_target_warnings(libglass)
target_link_libraries(libglass PUBLIC wpigui wpimath wpiutil fieldImages)

target_include_directories(
    libglass
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/lib/native/include>
        $<INSTALL_INTERFACE:${include_dest}/glass>
)

install(TARGETS libglass EXPORT libglass)
export(TARGETS libglass FILE libglass.cmake NAMESPACE libglass::)
install(DIRECTORY src/lib/native/include/ DESTINATION "${include_dest}/glass")

#
# libglassnt
#
file(GLOB_RECURSE libglassnt_src src/libnt/native/cpp/*.cpp)

add_library(libglassnt STATIC ${libglassnt_src})
set_target_properties(libglassnt PROPERTIES DEBUG_POSTFIX "d")
if(NOT MSVC)
    set_target_properties(libglassnt PROPERTIES OUTPUT_NAME "glassnt")
endif()

set_property(TARGET libglassnt PROPERTY FOLDER "libraries")

wpilib_target_warnings(libglassnt)
target_link_libraries(libglassnt PUBLIC ntcore libglass)

target_include_directories(
    libglassnt
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/libnt/native/include>
        $<INSTALL_INTERFACE:${include_dest}/glass>
)

install(TARGETS libglassnt EXPORT libglassnt)
export(TARGETS libglassnt FILE libglassnt.cmake NAMESPACE libglassnt::)
install(DIRECTORY src/libnt/native/include/ DESTINATION "${include_dest}/glass")

#
# glass application
#

configure_file(src/app/generate/WPILibVersion.cpp.in WPILibVersion.cpp)
generate_resources(src/app/native/resources generated/app/cpp GLASS glass glass_resources_src)

file(GLOB glass_src src/app/native/cpp/*.cpp ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.cpp)

if(WIN32)
    set(glass_rc src/app/native/win/glass.rc)
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE glass.icns)
    set(APP_ICON_MACOSX src/app/native/mac/glass.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

add_executable(glass ${glass_src} ${glass_resources_src} ${glass_rc} ${APP_ICON_MACOSX})
wpilib_link_macos_gui(glass)
wpilib_target_warnings(glass)
target_link_libraries(glass libglassnt libglass)

if(WIN32)
    set_target_properties(glass PROPERTIES WIN32_EXECUTABLE YES)
elseif(APPLE)
    set_target_properties(glass PROPERTIES MACOSX_BUNDLE YES OUTPUT_NAME "Glass")
endif()

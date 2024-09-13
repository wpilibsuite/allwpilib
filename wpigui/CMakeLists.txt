project(wpigui)

include(CompileWarnings)
include(LinkMacOSGUI)

file(GLOB wpigui_src src/main/native/cpp/*.cpp)
file(GLOB wpigui_windows_src src/main/native/directx11/*.cpp)
file(GLOB wpigui_mac_src src/main/native/metal/*.mm)
file(GLOB wpigui_unix_src src/main/native/opengl3/*.cpp)

add_library(wpigui STATIC ${wpigui_src})
set_target_properties(wpigui PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET wpigui PROPERTY FOLDER "libraries")

wpilib_target_warnings(wpigui)
target_link_libraries(wpigui PUBLIC imgui)

target_include_directories(
    wpigui
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/wpigui>
)

if(MSVC)
    target_sources(wpigui PRIVATE ${wpigui_windows_src})
elseif(APPLE)
    target_compile_options(wpigui PRIVATE -fobjc-arc)
    wpilib_link_macos_gui(wpigui)
    target_sources(wpigui PRIVATE ${wpigui_mac_src})
else()
    target_sources(wpigui PRIVATE ${wpigui_unix_src})
endif()

add_executable(wpiguidev src/dev/native/cpp/main.cpp)
wpilib_link_macos_gui(wpiguidev)
wpilib_target_warnings(wpiguidev)
target_link_libraries(wpiguidev wpigui)

install(TARGETS wpigui EXPORT wpigui)
export(TARGETS wpigui FILE wpigui.cmake NAMESPACE wpigui::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/wpigui")

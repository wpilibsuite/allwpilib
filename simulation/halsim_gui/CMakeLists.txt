project(halsim_gui)

include(CompileWarnings)
include(AddTest)
include(LinkMacOSGUI)

file(GLOB halsim_gui_src src/main/native/cpp/*.cpp)

add_library(halsim_gui SHARED ${halsim_gui_src})
wpilib_target_warnings(halsim_gui)
set_target_properties(halsim_gui PROPERTIES DEBUG_POSTFIX "d")

wpilib_link_macos_gui(halsim_gui)
target_link_libraries(halsim_gui PUBLIC hal wpimath PRIVATE libglassnt libglass)

target_include_directories(halsim_gui PRIVATE src/main/native/include)

set_property(TARGET halsim_gui PROPERTY FOLDER "libraries")

install(TARGETS halsim_gui EXPORT halsim_gui)
export(TARGETS halsim_gui FILE halsim_gui.cmake NAMESPACE halsim_gui::)

if(WPILIB_WITH_IMGUI_TESTS)
    wpilib_add_imgui_test(halsim_gui src/test/native/imgui)
    wpilib_link_macos_gui(halsim_gui_imgui_test)
    target_sources(halsim_gui_imgui_test PRIVATE ${halsim_gui_src})
    target_include_directories(halsim_gui_imgui_test PRIVATE src/main/native/include)
    target_link_libraries(halsim_gui_imgui_test hal wpimath libglassnt libglass)
endif()

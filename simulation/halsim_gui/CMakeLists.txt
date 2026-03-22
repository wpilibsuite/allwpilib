project(halsim_gui)

include(CompileWarnings)
include(LinkMacOSGUI)

file(GLOB halsim_gui_src src/main/native/cpp/*.cpp)

if(APPLE)
    file(GLOB halsim_gui_objcpp_src src/main/native/objcpp/*.mm)
    list(APPEND halsim_gui_src ${halsim_gui_objcpp_src})
endif()

add_library(halsim_gui SHARED ${halsim_gui_src})
wpilib_target_warnings(halsim_gui)
set_target_properties(halsim_gui PROPERTIES DEBUG_POSTFIX "d")

wpilib_link_macos_gui(halsim_gui)
target_link_libraries(halsim_gui PUBLIC hal wpimath PRIVATE libglassnt libglass)

if(APPLE)
    target_link_libraries(halsim_gui PRIVATE "-framework GameController")
endif()

target_include_directories(halsim_gui PRIVATE src/main/native/include)

set_property(TARGET halsim_gui PROPERTY FOLDER "libraries")

install(TARGETS halsim_gui EXPORT halsim_gui)
export(TARGETS halsim_gui FILE halsim_gui.cmake NAMESPACE halsim_gui::)

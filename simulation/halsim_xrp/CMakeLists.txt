project(halsim_xrp)

include(CompileWarnings)
include(LinkMacOSGUI)

file(GLOB halsim_xrp_src src/main/native/cpp/*.cpp)
if(NOT WPILIB_WITH_GUI)
    list(
        REMOVE_ITEM halsim_xrp_src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/cpp/HALSimXRPGui.cpp
    )
endif()

add_library(halsim_xrp SHARED ${halsim_xrp_src})
wpilib_target_warnings(halsim_xrp)
set_target_properties(halsim_xrp PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(halsim_xrp PUBLIC hal halsim_ws_core)
if(WPILIB_WITH_GUI)
    target_compile_definitions(halsim_xrp PRIVATE HALSIM_XRP_WITH_GUI)
    wpilib_link_macos_gui(halsim_xrp)
    target_link_libraries(halsim_xrp PRIVATE imgui libglass wpigui)
endif()

target_include_directories(halsim_xrp PRIVATE src/main/native/include)

set_property(TARGET halsim_xrp PROPERTY FOLDER "libraries")

install(TARGETS halsim_xrp EXPORT halsim_xrp)
export(TARGETS halsim_xrp FILE halsim_xrp.cmake NAMESPACE halsim_xrp::)

if(WPILIB_WITH_TESTS)
    wpilib_add_test(halsim_xrp src/test/native/cpp src/main/native/cpp/XRP.cpp)
    target_include_directories(halsim_xrp_test PRIVATE src/main/native/include)
    target_link_libraries(halsim_xrp_test wpinet)
endif()

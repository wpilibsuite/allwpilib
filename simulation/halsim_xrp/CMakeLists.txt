project(halsim_xrp)

include(CompileWarnings)

file(GLOB halsim_xrp_src src/main/native/cpp/*.cpp)

add_library(halsim_xrp SHARED ${halsim_xrp_src})
wpilib_target_warnings(halsim_xrp)
set_target_properties(halsim_xrp PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(halsim_xrp PUBLIC hal halsim_ws_core)

target_include_directories(halsim_xrp PRIVATE src/main/native/include)

set_property(TARGET halsim_xrp PROPERTY FOLDER "libraries")

install(TARGETS halsim_xrp EXPORT halsim_xrp)
export(TARGETS halsim_xrp FILE halsim_xrp.cmake NAMESPACE halsim_xrp::)

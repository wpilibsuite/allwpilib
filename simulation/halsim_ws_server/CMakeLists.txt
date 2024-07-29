project(halsim_ws_server)

include(CompileWarnings)

file(GLOB halsim_ws_server_src src/main/native/cpp/*.cpp)

add_library(halsim_ws_server SHARED ${halsim_ws_server_src})
wpilib_target_warnings(halsim_ws_server)
set_target_properties(halsim_ws_server PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(halsim_ws_server PUBLIC hal halsim_ws_core)

target_include_directories(halsim_ws_server PRIVATE src/main/native/include)

set_property(TARGET halsim_ws_server PROPERTY FOLDER "libraries")

install(TARGETS halsim_ws_server EXPORT halsim_ws_server)
export(TARGETS halsim_ws_server FILE halsim_ws_server.cmake NAMESPACE halsim_ws_server::)

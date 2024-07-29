project(halsim_ws_client)

include(CompileWarnings)

file(GLOB halsim_ws_client_src src/main/native/cpp/*.cpp)

add_library(halsim_ws_client SHARED ${halsim_ws_client_src})
wpilib_target_warnings(halsim_ws_client)
set_target_properties(halsim_ws_client PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(halsim_ws_client PUBLIC hal halsim_ws_core)

target_include_directories(halsim_ws_client PRIVATE src/main/native/include)

set_property(TARGET halsim_ws_client PROPERTY FOLDER "libraries")

install(TARGETS halsim_ws_client EXPORT halsim_ws_client)
export(TARGETS halsim_ws_client FILE halsim_ws_client.cmake NAMESPACE halsim_ws_client::)

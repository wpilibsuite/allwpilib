project(halsim_ws_core)

include(CompileWarnings)

file(GLOB halsim_ws_core_src src/main/native/cpp/*.cpp)

add_library(halsim_ws_core STATIC ${halsim_ws_core_src})
wpilib_target_warnings(halsim_ws_core)
set_target_properties(halsim_ws_core PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(halsim_ws_core PUBLIC hal wpinet)

target_include_directories(halsim_ws_core PUBLIC src/main/native/include)

set_property(TARGET halsim_ws_core PROPERTY FOLDER "libraries")

install(TARGETS halsim_ws_core EXPORT halsim_ws_core)
export(TARGETS halsim_ws_core FILE halsim_ws_core.cmake NAMESPACE halsim_ws_core::)

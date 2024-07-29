project(halsim_ds_socket)

include(CompileWarnings)

file(GLOB halsim_ds_socket_src src/main/native/cpp/*.cpp)

add_library(halsim_ds_socket SHARED ${halsim_ds_socket_src})
wpilib_target_warnings(halsim_ds_socket)
set_target_properties(halsim_ds_socket PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(halsim_ds_socket PUBLIC hal wpinet)

target_include_directories(halsim_ds_socket PRIVATE src/main/native/include)

set_property(TARGET halsim_ds_socket PROPERTY FOLDER "libraries")

install(TARGETS halsim_ds_socket EXPORT halsim_ds_socket)
export(TARGETS halsim_ds_socket FILE halsim_ds_socket.cmake NAMESPACE halsim_ds_socket::)

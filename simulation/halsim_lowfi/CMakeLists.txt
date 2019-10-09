project(halsim_lowfi)

include(CompileWarnings)

file(GLOB halsim_lowfi_src src/main/native/cpp/*.cpp)

add_library(halsim_lowfi MODULE ${halsim_lowfi_src})
wpilib_target_warnings(halsim_lowfi)
set_target_properties(halsim_lowfi PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(halsim_lowfi PUBLIC hal ntcore)

target_include_directories(halsim_lowfi PRIVATE src/main/native/include)

set_property(TARGET halsim_lowfi PROPERTY FOLDER "libraries")

install(TARGETS halsim_lowfi EXPORT halsim_lowfi DESTINATION "${main_lib_dest}")

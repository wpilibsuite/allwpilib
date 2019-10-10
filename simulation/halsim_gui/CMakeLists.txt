project(halsim_gui)

include(CompileWarnings)

file(GLOB halsim_gui_src src/main/native/cpp/*.cpp)

add_library(halsim_gui MODULE ${halsim_gui_src})
wpilib_target_warnings(halsim_gui)
set_target_properties(halsim_gui PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(halsim_gui PUBLIC hal PRIVATE imgui)

target_include_directories(halsim_gui PRIVATE src/main/native/include)

set_property(TARGET halsim_gui PROPERTY FOLDER "libraries")

install(TARGETS halsim_gui EXPORT halsim_gui DESTINATION "${main_lib_dest}")

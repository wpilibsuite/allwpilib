project(halsim_print)

include(CompileWarnings)

file(GLOB halsim_print_src src/main/native/cpp/*.cpp)

add_library(halsim_print MODULE ${halsim_print_src})
wpilib_target_warnings(halsim_print)
set_target_properties(halsim_print PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(halsim_print PUBLIC hal)

target_include_directories(halsim_print PRIVATE src/main/native/include)

set_property(TARGET halsim_print PROPERTY FOLDER "libraries")

install(TARGETS halsim_print EXPORT halsim_print DESTINATION "${main_lib_dest}")

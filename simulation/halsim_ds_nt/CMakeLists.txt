project(halsim_ds_nt)

include(CompileWarnings)

file(GLOB halsim_ds_nt_src src/main/native/cpp/*.cpp)

add_library(halsim_ds_nt MODULE ${halsim_ds_nt_src})
wpilib_target_warnings(halsim_ds_nt)
set_target_properties(halsim_ds_nt PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(halsim_ds_nt PUBLIC hal ntcore)

target_include_directories(halsim_ds_nt PRIVATE src/main/native/include)

set_property(TARGET halsim_ds_nt PROPERTY FOLDER "libraries")

install(TARGETS halsim_ds_nt EXPORT halsim_ds_nt DESTINATION "${main_lib_dest}")

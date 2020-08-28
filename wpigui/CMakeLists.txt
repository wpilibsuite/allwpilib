project(wpigui)

include(CompileWarnings)

file(GLOB wpigui_src src/main/native/cpp/*.cpp)
file(GLOB wpigui_windows_src src/main/native/directx11/*.cpp)
file(GLOB wpigui_mac_src src/main/native/metal/*.mm)
file(GLOB wpigui_unix_src src/main/native/opengl3/*.cpp)

add_library(wpigui ${wpigui_src})
set_target_properties(wpigui PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET wpigui PROPERTY FOLDER "libraries")

target_compile_features(wpigui PUBLIC cxx_std_17)
wpilib_target_warnings(wpigui)
target_link_libraries(wpigui imgui)

target_include_directories(wpigui PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
                            $<INSTALL_INTERFACE:${include_dest}/wpigui>)

if (MSVC)
    target_sources(wpigui PRIVATE ${wpigui_windows_src})
else()
    if (APPLE)
        target_sources(wpigui PRIVATE ${wpigui_mac_src})
    else()
        target_sources(wpigui PRIVATE ${wpigui_unix_src})
    endif()
endif()

install(TARGETS wpigui EXPORT wpigui DESTINATION "${main_lib_dest}")
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/wpigui")

#if (MSVC OR FLAT_INSTALL_WPILIB)
#    set (wpigui_config_dir ${wpilib_dest})
#else()
#    set (wpigui_config_dir share/wpigui)
#endif()

#configure_file(wpigui-config.cmake.in ${CMAKE_BINARY_DIR}/wpigui-config.cmake )
#install(FILES ${CMAKE_BINARY_DIR}/wpigui-config.cmake DESTINATION ${wpigui_config_dir})
#install(EXPORT wpigui DESTINATION ${wpigui_config_dir})

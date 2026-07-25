project(wpigui)

include(CompileWarnings)

file(GLOB wpigui_src src/main/native/cpp/*.cpp)

add_library(wpigui STATIC ${wpigui_src})
set_target_properties(wpigui PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET wpigui PROPERTY FOLDER "libraries")

wpilib_target_warnings(wpigui)
target_link_libraries(wpigui PUBLIC imgui)

target_include_directories(
    wpigui
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/wpigui>
)

add_executable(wpiguidev src/dev/native/cpp/main.cpp)
wpilib_target_warnings(wpiguidev)
target_link_libraries(wpiguidev wpigui)

install(TARGETS wpigui EXPORT wpigui)
export(TARGETS wpigui FILE wpigui.cmake NAMESPACE wpigui::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/wpigui")

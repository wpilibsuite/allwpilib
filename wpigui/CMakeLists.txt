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

if(WPILIB_WITH_EXECUTABLES)
    add_executable(wpiguidev src/dev/native/cpp/main.cpp)
    wpilib_target_warnings(wpiguidev)
    target_link_libraries(wpiguidev wpigui)
endif()

if(WPILIB_WITH_IMGUI_TESTS)
    add_library(wpigui_for_tests STATIC ${wpigui_src})
    set_target_properties(wpigui_for_tests PROPERTIES DEBUG_POSTFIX "d")
    set_property(TARGET wpigui_for_tests PROPERTY FOLDER "tests")
    wpilib_target_warnings(wpigui_for_tests)
    target_link_libraries(wpigui_for_tests PUBLIC imgui_test_engine)
    target_include_directories(
        wpigui_for_tests
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
    )

    add_library(wpigui_test_runner STATIC src/test/native/cpp/GuiTestEngineRunner.cpp)
    set_property(TARGET wpigui_test_runner PROPERTY FOLDER "tests")
    wpilib_target_warnings(wpigui_test_runner)
    target_link_libraries(wpigui_test_runner PUBLIC wpigui_for_tests imgui_test_engine)
    target_include_directories(
        wpigui_test_runner
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/test/native/include>
    )
endif()

install(TARGETS wpigui EXPORT wpigui)
export(TARGETS wpigui FILE wpigui.cmake NAMESPACE wpigui::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/wpigui")

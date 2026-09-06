# Add imgui directly to our build.
file(GLOB imgui_sources imgui/cpp/*.cpp imgui/cpp/misc/cpp/*.cpp)
file(GLOB implot_sources implot/cpp/*.cpp)
file(GLOB fonts_sources generated/fonts/src/*.cpp)
set(imgui_all_sources
    ${imgui_sources}
    ${implot_sources}
    ${fonts_sources}
    imgui/cpp/backends/imgui_impl_sdl3.cpp
    imgui/cpp/backends/imgui_impl_sdlgpu3.cpp
    imgui/cpp/backends/imgui_impl_sdlrenderer3.cpp
    stb/cpp/stb_image.cpp
)
if(MSVC)
    add_library(imgui STATIC ${imgui_all_sources})
else()
    add_library(imgui ${imgui_all_sources})
endif()

function(wpilib_configure_imgui_target target)
    target_link_libraries(${target} PUBLIC SDL3::SDL3)
    target_include_directories(
        ${target}
        PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui/include>"
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui/include/misc/cpp>"
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui/include/backends>"
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/implot/include>"
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated/fonts/include>"
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/stb/include>"
        PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    )

    target_compile_features(${target} PUBLIC cxx_std_23)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 20.0)
        target_compile_options(${target} PUBLIC -Wno-nontrivial-memcall)
    endif()
endfunction()

wpilib_configure_imgui_target(imgui)

if(WPILIB_WITH_IMGUI_TESTS)
    set(imgui_test_engine_dir "${imgui_test_engine_SOURCE_DIR}/imgui_test_engine")
    add_library(imgui_for_tests STATIC ${imgui_all_sources})
    set_property(TARGET imgui_for_tests PROPERTY FOLDER "tests")
    wpilib_configure_imgui_target(imgui_for_tests)
    target_compile_definitions(
        imgui_for_tests
        PUBLIC
            IMGUI_ENABLE_TEST_ENGINE
            IMGUI_TEST_ENGINE_ENABLE_CAPTURE=0
            IMGUI_TEST_ENGINE_ENABLE_COROUTINE_STDTHREAD_IMPL=1
    )

    add_library(
        imgui_test_engine
        STATIC
        ${imgui_test_engine_dir}/imgui_capture_tool.cpp
        ${imgui_test_engine_dir}/imgui_te_context.cpp
        ${imgui_test_engine_dir}/imgui_te_coroutine.cpp
        ${imgui_test_engine_dir}/imgui_te_engine.cpp
        ${imgui_test_engine_dir}/imgui_te_exporters.cpp
        ${imgui_test_engine_dir}/imgui_te_perftool.cpp
        ${imgui_test_engine_dir}/imgui_te_ui.cpp
        ${imgui_test_engine_dir}/imgui_te_utils.cpp
    )
    target_link_libraries(imgui_test_engine PUBLIC imgui_for_tests)
    target_include_directories(
        imgui_test_engine
        PUBLIC
            "$<BUILD_INTERFACE:${imgui_test_engine_SOURCE_DIR}>"
            "$<BUILD_INTERFACE:${imgui_test_engine_dir}>"
    )
    target_compile_features(imgui_test_engine PUBLIC cxx_std_23)
endif()

install(TARGETS imgui EXPORT imgui)
export(TARGETS imgui FILE imgui.cmake NAMESPACE imgui::)

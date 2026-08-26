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
target_link_libraries(imgui PUBLIC SDL3::SDL3)
target_include_directories(
    imgui
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui/include/misc/cpp>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui/include/backends>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/implot/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated/fonts/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/stb/include>"
    PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
)

target_compile_features(imgui PUBLIC cxx_std_23)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 20.0)
  target_compile_options(imgui PUBLIC -Wno-nontrivial-memcall)
endif()

install(TARGETS imgui EXPORT imgui)
export(TARGETS imgui FILE imgui.cmake NAMESPACE imgui::)

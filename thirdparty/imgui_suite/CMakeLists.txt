include(LinkMacOSGUI)

set(SAVE_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory("glfw")
set(BUILD_SHARED_LIBS ${SAVE_BUILD_SHARED_LIBS})
export(TARGETS glfw FILE glfw.cmake NAMESPACE glfw::)

# Add imgui directly to our build.
file(GLOB imgui_sources imgui/cpp/*.cpp imgui/cpp/misc/cpp/*.cpp)
file(GLOB implot_sources implot/cpp/*.cpp)
file(GLOB fonts_sources generated/fonts/src/*.cpp)
set(imgui_all_sources
    ${imgui_sources}
    ${implot_sources}
    ${fonts_sources}
    imgui/cpp/backends/imgui_impl_glfw.cpp
    imgui/cpp/backends/imgui_impl_opengl3.cpp
    generated/gl3w/src/gl3w.c
    stb/cpp/stb_image.cpp
)
if(MSVC)
    add_library(imgui STATIC ${imgui_all_sources})
else()
    add_library(imgui ${imgui_all_sources})
endif()
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GL3W)
if(MSVC)
    target_sources(imgui PRIVATE imgui/cpp/backends/imgui_impl_dx11.cpp)
else()
    if(APPLE)
        target_compile_options(imgui PRIVATE -fobjc-arc)
        wpilib_link_macos_gui(imgui)
        target_sources(imgui PRIVATE imgui/cpp/backends/imgui_impl_metal.mm)
    else()
        #target_sources(imgui PRIVATE imgui/cpp/backends/imgui_impl_opengl3.cpp)
    endif()
endif()
target_link_libraries(imgui PUBLIC glfw)
target_include_directories(
    imgui
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui/include/misc/cpp>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui/include/backends>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/implot/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated/gl3w/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/generated/fonts/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/stb/include>"
    PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
)

target_compile_features(imgui PUBLIC cxx_std_20)

install(TARGETS imgui EXPORT imgui)
export(TARGETS imgui FILE imgui.cmake NAMESPACE imgui::)

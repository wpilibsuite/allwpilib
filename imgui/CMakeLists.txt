INCLUDE(FetchContent)

FetchContent_Declare(
    glfw3
    GIT_REPOSITORY  https://github.com/glfw/glfw.git
    GIT_TAG         6b57e08bb0078c9834889eab871bac2368198c15
)
FetchContent_Declare(
    gl3w
    GIT_REPOSITORY  https://github.com/skaslev/gl3w
    GIT_TAG         5f8d7fd191ba22ff2b60c1106d7135bb9a335533
)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY  https://github.com/ocornut/imgui.git
    GIT_TAG         aceab9a877de0258d19d29a5d87a51b63a8999bf
)
FetchContent_Declare(
    implot
    GIT_REPOSITORY  https://github.com/epezent/implot.git
    GIT_TAG         e80e42e8b4136ddb84ccfe04fa28d0c745828952
)
FetchContent_Declare(
    proggyfonts
    URL         https://github.com/bluescan/proggyfonts/archive/refs/tags/v1.1.5.zip
    URL_HASH    SHA256=260c5311b655ef1e73bf38947a82d37f14ed51522d3bcaf1b466c2d6225b11bb
)
FetchContent_Declare(
    stb
    GIT_REPOSITORY  https://github.com/nothings/stb.git
    GIT_TAG         c9064e317699d2e495f36ba4f9ac037e88ee371a
)

FetchContent_MakeAvailable(
    imgui
    implot
    proggyfonts
    stb
)

# Build font
add_executable(imgui_font_bin2c ${imgui_SOURCE_DIR}/misc/fonts/binary_to_compressed_c.cpp)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ProggyDotted.inc
  COMMAND imgui_font_bin2c
  ARGS "${proggyfonts_SOURCE_DIR}/ProggyDotted/ProggyDotted Regular.ttf" ProggyDotted > ${CMAKE_CURRENT_BINARY_DIR}/ProggyDotted.inc
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  MAIN_DEPENDENCY "${proggyfonts_SOURCE_DIR}/ProggyDotted/ProggyDotted Regular.ttf"
  VERBATIM
)
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/imgui_ProggyDotted.cpp
    CONTENT "#include \"imgui_ProggyDotted.h\"\n#include \"ProggyDotted.inc\"\nImFont* ImGui::AddFontProggyDotted(ImGuiIO& io, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges) {\n  return io.Fonts->AddFontFromMemoryCompressedTTF(ProggyDotted_compressed_data, ProggyDotted_compressed_size, size_pixels, font_cfg, glyph_ranges);\n}\n"
)
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/imgui_ProggyDotted.h
    CONTENT "#pragma once\n#include \"imgui.h\"\nnamespace ImGui {\nImFont* AddFontProggyDotted(ImGuiIO& io, float size_pixels, const ImFontConfig* font_cfg = nullptr, const ImWchar* glyph_ranges = nullptr);\n}\n"
)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/imgui_ProggyDotted.cpp
    PROPERTIES OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ProggyDotted.inc)

# stb_image
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/stb_image.cpp
    CONTENT "#define STBI_WINDOWS_UTF8\n#define STB_IMAGE_IMPLEMENTATION\n#include \"stb_image.h\"\n"
)

# Add glfw directly to our build.
FetchContent_GetProperties(glfw3)
if(NOT glfw3_POPULATED)
    FetchContent_Populate(glfw3)
    set(SAVE_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(BUILD_SHARED_LIBS OFF)
    set(GLFW_INSTALL OFF)
    add_subdirectory(${glfw3_SOURCE_DIR} ${glfw3_BINARY_DIR} EXCLUDE_FROM_ALL)
    set_property(TARGET glfw PROPERTY POSITION_INDEPENDENT_CODE ON)
    set(BUILD_SHARED_LIBS ${SAVE_BUILD_SHARED_LIBS})
endif()

# Don't use gl3w CMakeLists.txt due to https://github.com/skaslev/gl3w/issues/66
FetchContent_GetProperties(gl3w)
if(NOT gl3w_POPULATED)
    FetchContent_Populate(gl3w)
    find_package(Python COMPONENTS Interpreter Development REQUIRED)
    add_custom_command(
        OUTPUT
            "${gl3w_BINARY_DIR}/src/gl3w.c"
            "${gl3w_BINARY_DIR}/include/GL/gl3w.h"
            "${gl3w_BINARY_DIR}/include/GL/glcorearb.h"
            "${gl3w_BINARY_DIR}/include/KHR/khrplatform.h"
        COMMAND "${PYTHON_EXECUTABLE}" ${gl3w_SOURCE_DIR}/gl3w_gen.py
        DEPENDS ${gl3w_SOURCE_DIR}/gl3w_gen.py
        WORKING_DIRECTORY ${gl3w_BINARY_DIR}
    )
endif()

# Add imgui directly to our build.
file(GLOB imgui_sources ${imgui_SOURCE_DIR}/*.cpp ${imgui_SOURCE_DIR}/misc/cpp/*.cpp)
file(GLOB implot_sources ${implot_SOURCE_DIR}/*.cpp)
add_library(imgui STATIC
    ${imgui_sources}
    ${implot_sources}
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${gl3w_BINARY_DIR}/src/gl3w.c
    ${CMAKE_CURRENT_BINARY_DIR}/imgui_ProggyDotted.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/stb_image.cpp
)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GL3W)
if (MSVC)
    target_sources(imgui PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp)
else()
    if (APPLE)
        target_compile_options(imgui PRIVATE -fobjc-arc)
        set_target_properties(imgui PROPERTIES LINK_FLAGS "-framework Metal -framework QuartzCore")
        target_sources(imgui PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_metal.mm)
    else()
        #target_sources(imgui PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
    endif()
endif()
target_link_libraries(imgui PUBLIC glfw)
target_include_directories(imgui PUBLIC
    "$<BUILD_INTERFACE:${imgui_SOURCE_DIR}>"
    "$<BUILD_INTERFACE:${imgui_SOURCE_DIR}/misc/cpp>"
    "$<BUILD_INTERFACE:${implot_SOURCE_DIR}>"
    "$<BUILD_INTERFACE:${imgui_SOURCE_DIR}/backends>"
    "$<BUILD_INTERFACE:${gl3w_BINARY_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<BUILD_INTERFACE:${stb_SOURCE_DIR}>"
)

set_property(TARGET imgui PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(imgui PUBLIC cxx_std_20)

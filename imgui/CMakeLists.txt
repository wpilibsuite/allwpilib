# Download and unpack imgui at configure time
configure_file(CMakeLists.txt.in imgui-download/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/imgui-download )
if(result)
  message(FATAL_ERROR "CMake step for imgui failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/imgui-download )
if(result)
  message(FATAL_ERROR "Build step for imgui failed: ${result}")
endif()

# Build font
add_executable(imgui_font_bin2c ${CMAKE_CURRENT_BINARY_DIR}/imgui-src/misc/fonts/binary_to_compressed_c.cpp)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ProggyDotted.inc
  COMMAND imgui_font_bin2c
  ARGS "${CMAKE_CURRENT_BINARY_DIR}/proggyfonts-src/ProggyDotted/ProggyDotted Regular.ttf" ProggyDotted > ${CMAKE_CURRENT_BINARY_DIR}/ProggyDotted.inc
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  MAIN_DEPENDENCY "${CMAKE_CURRENT_BINARY_DIR}/proggyfonts-src/ProggyDotted/ProggyDotted Regular.ttf"
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

# Add imgui directly to our build.
set(SAVE_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/glfw-src
                 ${CMAKE_CURRENT_BINARY_DIR}/glfw-build
                 EXCLUDE_FROM_ALL)
set_property(TARGET glfw PROPERTY POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS ${SAVE_BUILD_SHARED_LIBS})
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/gl3w-src
                 ${CMAKE_CURRENT_BINARY_DIR}/gl3w-build
                 EXCLUDE_FROM_ALL)

set(imgui_srcdir ${CMAKE_CURRENT_BINARY_DIR}/imgui-src)
file(GLOB imgui_sources ${imgui_srcdir}/*.cpp ${imgui_srcdir}/misc/cpp/*.cpp)
set(implot_srcdir ${CMAKE_CURRENT_BINARY_DIR}/implot-src)
file(GLOB implot_sources ${implot_srcdir}/*.cpp)
add_library(imgui STATIC ${imgui_sources} ${implot_sources} ${imgui_srcdir}/examples/imgui_impl_glfw.cpp ${imgui_srcdir}/examples/imgui_impl_opengl3.cpp ${CMAKE_CURRENT_BINARY_DIR}/imgui_ProggyDotted.cpp ${CMAKE_CURRENT_BINARY_DIR}/stb_image.cpp)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GL3W)
if (MSVC)
    target_sources(imgui PRIVATE ${imgui_srcdir}/examples/imgui_impl_dx11.cpp)
else()
    if (APPLE)
        target_compile_options(imgui PRIVATE -fobjc-arc)
        set_target_properties(imgui PROPERTIES LINK_FLAGS "-framework Metal -framework QuartzCore")
        target_sources(imgui PRIVATE ${imgui_srcdir}/examples/imgui_impl_metal.mm)
    else()
        #target_sources(imgui PRIVATE ${imgui_srcdir}/examples/imgui_impl_opengl3.cpp)
    endif()
endif()
target_link_libraries(imgui PUBLIC gl3w glfw)
target_include_directories(imgui PUBLIC "$<BUILD_INTERFACE:${imgui_srcdir}>" "$<BUILD_INTERFACE:${imgui_srcdir}/misc/cpp>" "$<BUILD_INTERFACE:${implot_srcdir}>" "$<BUILD_INTERFACE:${imgui_srcdir}/examples>" "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>" "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/stb-src>")

set_property(TARGET imgui PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(imgui PUBLIC cxx_std_17)

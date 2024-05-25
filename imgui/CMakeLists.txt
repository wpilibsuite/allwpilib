include(FetchContent)

fetchcontent_declare(
    glfw3
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 6b57e08bb0078c9834889eab871bac2368198c15
)
fetchcontent_declare(
    gl3w
    GIT_REPOSITORY https://github.com/skaslev/gl3w
    GIT_TAG 5f8d7fd191ba22ff2b60c1106d7135bb9a335533
)
fetchcontent_declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    # docking branch
    GIT_TAG 64b1e448d20c9be9275af731c34b4c7bf14a8e95
)
fetchcontent_declare(
    implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    # v0.16
    GIT_TAG 18c72431f8265e2b0b5378a3a73d8a883b2175ff
)
fetchcontent_declare(
    fonts
    URL https://github.com/wpilibsuite/thirdparty-fonts/releases/download/v0.2/fonts.zip
    URL_HASH SHA256=cedf365657fab0770e11f72d49e4f0f889f564d2e635a4d214029d0ab6bcd324
)
fetchcontent_declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG c9064e317699d2e495f36ba4f9ac037e88ee371a
)

fetchcontent_makeavailable(imgui implot fonts stb)

# Add glfw directly to our build.
fetchcontent_getproperties(glfw3)
if(NOT glfw3_POPULATED)
    fetchcontent_populate(glfw3)
    set(SAVE_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
    set(BUILD_SHARED_LIBS OFF)
    set(GLFW_INSTALL OFF)
    add_subdirectory(${glfw3_SOURCE_DIR} ${glfw3_BINARY_DIR} EXCLUDE_FROM_ALL)
    set_property(TARGET glfw PROPERTY POSITION_INDEPENDENT_CODE ON)
    set(BUILD_SHARED_LIBS ${SAVE_BUILD_SHARED_LIBS})
    export(TARGETS glfw FILE glfw.cmake NAMESPACE glfw::)
endif()

# Don't use gl3w CMakeLists.txt due to https://github.com/skaslev/gl3w/issues/66
fetchcontent_getproperties(gl3w)
if(NOT gl3w_POPULATED)
    fetchcontent_populate(gl3w)
endif()
if(NOT EXISTS "${gl3w_BINARY_DIR}/src/gl3w.c")
    find_package(Python COMPONENTS Interpreter Development REQUIRED)
    execute_process(
        COMMAND "${Python_EXECUTABLE}" ${gl3w_SOURCE_DIR}/gl3w_gen.py "--root=${gl3w_BINARY_DIR}"
        WORKING_DIRECTORY ${gl3w_BINARY_DIR}
    )
endif()

# Add imgui directly to our build.
file(GLOB imgui_sources ${imgui_SOURCE_DIR}/*.cpp ${imgui_SOURCE_DIR}/misc/cpp/*.cpp)
file(GLOB implot_sources ${implot_SOURCE_DIR}/*.cpp)
file(GLOB fonts_sources ${fonts_SOURCE_DIR}/src/*.cpp)
set(imgui_all_sources
    ${imgui_sources}
    ${implot_sources}
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${gl3w_BINARY_DIR}/src/gl3w.c
    ${fonts_sources}
    src/stb_image.cpp
)
if(MSVC)
    add_library(imgui STATIC ${imgui_all_sources})
else()
    add_library(imgui ${imgui_all_sources})
endif()
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GL3W)
if(MSVC)
    target_sources(imgui PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp)
else()
    if(APPLE)
        target_compile_options(imgui PRIVATE -fobjc-arc)
        set_target_properties(imgui PROPERTIES LINK_FLAGS "-framework Metal -framework QuartzCore")
        target_sources(imgui PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_metal.mm)
    else()
        #target_sources(imgui PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
    endif()
endif()
target_link_libraries(imgui PUBLIC glfw)
target_include_directories(
    imgui
    PUBLIC
        "$<BUILD_INTERFACE:${imgui_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${imgui_SOURCE_DIR}/misc/cpp>"
        "$<BUILD_INTERFACE:${implot_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${imgui_SOURCE_DIR}/backends>"
        "$<BUILD_INTERFACE:${gl3w_BINARY_DIR}/include>"
        "$<BUILD_INTERFACE:${fonts_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${stb_SOURCE_DIR}>"
    PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
)

set_property(TARGET imgui PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(imgui PUBLIC cxx_std_20)

install(TARGETS imgui EXPORT imgui)
export(TARGETS imgui FILE imgui.cmake NAMESPACE imgui::)

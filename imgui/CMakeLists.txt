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

# Add imgui directly to our build.
set(SAVE_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/glfw-src
                 ${CMAKE_CURRENT_BINARY_DIR}/glfw-build
                 EXCLUDE_FROM_ALL)
set_property(TARGET glfw PROPERTY POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS ${SAVE_BUILD_SHARED_LIBS})
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/gl3w-src
                 ${CMAKE_CURRENT_BINARY_DIR}/gl3w-build
                 EXCLUDE_FROM_ALL)

set(imgui_srcdir ${CMAKE_CURRENT_BINARY_DIR}/imgui-src)
file(GLOB imgui_sources ${imgui_srcdir}/*.cpp)
add_library(imgui STATIC ${imgui_sources} ${imgui_srcdir}/examples/imgui_impl_glfw.cpp ${imgui_srcdir}/examples/imgui_impl_opengl3.cpp)
target_link_libraries(imgui PUBLIC gl3w glfw)
target_include_directories(imgui PUBLIC "$<BUILD_INTERFACE:${imgui_srcdir}>" "$<BUILD_INTERFACE:${imgui_srcdir}/examples>")

set_property(TARGET imgui PROPERTY POSITION_INDEPENDENT_CODE ON)

project(cscore)

include(SubDirList)
include(CompileWarnings)
include(AddTest)
include(LinkMacOSGUI)

file(GLOB cscore_native_src src/main/native/cpp/*.cpp)
file(GLOB cscore_linux_src src/main/native/linux/*.cpp)
file(GLOB cscore_osx_src src/main/native/osx/*.cpp)
file(GLOB cscore_osx_objc_src src/main/native/objcpp/*.mm)
file(GLOB cscore_windows_src src/main/native/windows/*.cpp)

add_library(cscore ${cscore_native_src})
set_target_properties(cscore PROPERTIES DEBUG_POSTFIX "d")

if(APPLE)
    target_sources(cscore PRIVATE ${cscore_osx_src} ${cscore_osx_objc_src})
    target_compile_options(cscore PRIVATE "-fobjc-arc")
    set_target_properties(
        cscore
        PROPERTIES
            LINK_FLAGS
                "-framework CoreFoundation -framework AVFoundation -framework Foundation -framework CoreMedia -framework CoreVideo -framework IOKit"
    )
elseif(MSVC)
    target_sources(cscore PRIVATE ${cscore_windows_src})
    target_compile_definitions(cscore PUBLIC -DNOMINMAX)
    target_compile_definitions(cscore PRIVATE -D_CRT_SECURE_NO_WARNINGS)
else()
    target_sources(cscore PRIVATE ${cscore_linux_src})
endif()

target_include_directories(
    cscore
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<INSTALL_INTERFACE:${include_dest}/cscore>
    PRIVATE src/main/native/cpp
)
target_include_directories(cscore SYSTEM PUBLIC ${OpenCV_INCLUDE_DIRS})
wpilib_target_warnings(cscore)
target_link_libraries(cscore PUBLIC wpinet wpiutil ${OpenCV_LIBS})

set_property(TARGET cscore PROPERTY FOLDER "libraries")

install(TARGETS cscore EXPORT cscore)
export(TARGETS cscore FILE cscore.cmake NAMESPACE cscore::)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/cscore")

configure_file(cscore-config.cmake.in ${WPILIB_BINARY_DIR}/cscore-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/cscore-config.cmake DESTINATION share/cscore)
install(EXPORT cscore DESTINATION share/cscore)

subdir_list(cscore_examples "${CMAKE_CURRENT_SOURCE_DIR}/examples")
foreach(example ${cscore_examples})
    file(GLOB cscore_example_src examples/${example}/*.cpp)
    unset(add_libs)
    if(${example} STREQUAL "usbviewer")
        if(TARGET wpigui)
            set(add_libs wpigui)
        else()
            unset(cscore_example_src)
        endif()
    endif()
    if(cscore_example_src)
        add_executable(cscore_${example} ${cscore_example_src})
        wpilib_target_warnings(cscore_${example})

        if(${example} STREQUAL "usbviewer")
            wpilib_link_macos_gui(cscore_${example})
        endif()

        target_link_libraries(cscore_${example} cscore ${add_libs})
        set_property(TARGET cscore_${example} PROPERTY FOLDER "examples")
    endif()
endforeach()

if(WPILIB_WITH_TESTS)
    wpilib_add_test_catch2(cscore src/test/native/cpp)
    target_link_libraries(cscore_test cscore)
endif()

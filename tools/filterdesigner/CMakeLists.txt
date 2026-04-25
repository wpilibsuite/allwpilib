project(filterdesigner)

include(CompileWarnings)
include(LinkMacOSGUI)
include(AddTest)

configure_file(src/main/generate/WPILibVersion.cpp.in WPILibVersion.cpp)

file(
    GLOB_RECURSE filterdesigner_src
    src/main/native/cpp/*.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.cpp
)

add_executable(filterdesigner ${filterdesigner_src})
if(MSVC)
    target_compile_options(filterdesigner PRIVATE /utf-8)
endif()
wpilib_link_macos_gui(filterdesigner)
wpilib_target_warnings(filterdesigner)
target_include_directories(
    filterdesigner
    PRIVATE src/main/native/cpp src/main/native/include src/main/native/thirdparty/pocketfft
)
target_link_libraries(
    filterdesigner
    wpimath
    libglass
    ntcore
    wpinet
    datalog
)

if(WIN32)
    set_target_properties(filterdesigner PROPERTIES WIN32_EXECUTABLE YES)
elseif(APPLE)
    set_target_properties(filterdesigner PROPERTIES MACOSX_BUNDLE YES OUTPUT_NAME "FilterDesigner")
endif()

if(WITH_TESTS)
    wpilib_add_test(filterdesigner src/test/native/cpp)
    wpilib_link_macos_gui(filterdesigner_test)
    target_sources(filterdesigner_test PRIVATE ${filterdesigner_src})
    target_compile_definitions(filterdesigner_test PRIVATE RUNNING_FILTERDESIGNER_TESTS)
    if(MSVC)
        target_compile_options(filterdesigner_test PRIVATE /utf-8)
    endif()
    target_include_directories(
        filterdesigner_test
        PRIVATE src/main/native/cpp src/main/native/include src/main/native/thirdparty/pocketfft
    )
    target_link_libraries(
        filterdesigner_test
        wpimath
        libglass
        ntcore
        wpinet
        datalog
        googletest
    )
endif()

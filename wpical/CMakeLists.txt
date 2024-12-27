project(wpical LANGUAGES C CXX Fortran)

include(CompileWarnings)
include(GenResources)
include(LinkMacOSGUI)
include(AddTest)

configure_file(src/main/generate/WPILibVersion.cpp.in WPILibVersion.cpp)
generate_resources(src/main/native/resources generated/main/cpp WPIcal wpical wpical_resources_src)

file(GLOB wpical_src src/main/native/cpp/*.cpp ${CMAKE_CURRENT_BINARY_DIR}/WPILibVersion.cpp)
file(
    GLOB_RECURSE wpical_thirdparty_src
    src/main/native/thirdparty/libdogleg/src/*.cpp
    src/main/native/thirdparty/mrcal/src/*.c
    src/main/native/thirdparty/mrcal/src/*.cpp
    src/main/native/thirdparty/mrcal_java/src/*.cpp
)

if(WIN32)
    set(wpical_rc src/main/native/win/wpical.rc)
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE wpical.icns)
    set(APP_ICON_MACOSX src/main/native/mac/wpical.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

add_executable(
    wpical
    ${wpical_src}
    ${wpical_thirdparty_src}
    ${wpical_resources_src}
    ${wpical_rc}
    ${APP_ICON_MACOSX}
)
wpilib_link_macos_gui(wpical)
wpilib_target_warnings(wpical)
target_include_directories(
    wpical
    PRIVATE
        src/main/native/include
        src/main/native/thirdparty/libdogleg/include
        src/main/native/thirdparty/mrcal/include
        src/main/native/thirdparty/mrcal_java/include
)

if(MSVC)
    set(compile_flags
        /wd4047
        /wd4098
        /wd4267
        /wd4068
        /wd4101
        /wd4200
        /wd4576
        /wd4715
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(compile_flags
        -Wno-format-nonliteral
        -Wno-unused-variable
        -Wno-missing-field-initializers
        -Wno-gnu-anonymous-struct
        -Wno-c99-extensions
        -Wno-zero-length-array
        -Wno-nested-anon-types
        -Wno-sign-compare
        -Wno-unused-function
        -Wno-missing-braces
        -Wno-null-conversion
        -Wno-unused-but-set-variable
    )
else()
    set(compile_flags
        -Wno-format-nonliteral
        -Wno-unused-variable
        -Wno-unused-function
        -Wno-sign-compare
        -Wno-missing-field-initializers
    )
endif()
target_compile_options(wpical PRIVATE ${compile_flags})

find_package(OpenCV REQUIRED)
find_package(Ceres CONFIG REQUIRED)
target_link_libraries(
    wpical
    apriltag
    ${OpenCV_LIBS}
    wpigui
    wpiutil
    Ceres::ceres
)

if(WIN32)
    set_target_properties(wpical PROPERTIES WIN32_EXECUTABLE YES)
elseif(APPLE)
    set_target_properties(wpical PROPERTIES MACOSX_BUNDLE YES OUTPUT_NAME "wpical")
endif()

if(WITH_TESTS)
    wpilib_add_test(wpical src/test/native/cpp)
    wpilib_link_macos_gui(wpical_test)
    target_sources(wpical_test PRIVATE ${wpical_src} ${wpical_thirdparty_src})
    target_compile_definitions(wpical_test PRIVATE RUNNING_WPICAL_TESTS)

    if(MSVC)
        target_compile_options(wpical_test PRIVATE /utf-8)
    endif()

    target_compile_options(wpical_test PRIVATE ${compile_flags})
    target_include_directories(
        wpical_test
        PRIVATE
            src/main/native/include
            src/main/native/thirdparty/libdogleg/include
            src/main/native/thirdparty/mrcal/include
            src/main/native/thirdparty/mrcal_java/include
    )
    target_link_libraries(
        wpical_test
        googletest
        apriltag
        ${OpenCV_LIBS}
        wpigui
        wpiutil
        Ceres::ceres
    )
endif()

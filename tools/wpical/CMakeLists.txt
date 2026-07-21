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
        src/main/native/thirdparty/mrcal/generated
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

set_source_files_properties(${wpical_thirdparty_src} PROPERTIES COMPILE_OPTIONS "${compile_flags}")

find_package(OpenCV REQUIRED)
find_package(CHOLMOD CONFIG REQUIRED)
find_package(SuiteSparse_config CONFIG REQUIRED)
find_package(Ceres CONFIG REQUIRED)

# use static libs for CHOLMOD/SuiteSparse if possible
if(TARGET SuiteSparse::CHOLMOD_static)
    set(cholmod_target SuiteSparse::CHOLMOD_static)
else()
    set(cholmod_target SuiteSparse::CHOLMOD)
endif()

if(TARGET SuiteSparse::SuiteSparseConfig_static)
    set(suitesparse_target SuiteSparse::SuiteSparseConfig_static)
else()
    set(suitesparse_target SuiteSparse::SuiteSparseConfig)
endif()

target_link_libraries(
    wpical
    apriltag
    wpimath
    ${OpenCV_LIBS}
    libglass
    wpigui
    wpiutil
    ${cholmod_target}
    ${suitesparse_target}
    Ceres::ceres
)

target_compile_definitions(wpical PUBLIC OPENCV_DISABLE_EIGEN_TENSOR_SUPPORT)
target_compile_definitions(wpical PUBLIC PROJECT_ROOT_PATH="${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/assets")

if(WIN32)
    set_target_properties(wpical PROPERTIES WIN32_EXECUTABLE YES)
elseif(APPLE)
    set_target_properties(wpical PROPERTIES MACOSX_BUNDLE YES OUTPUT_NAME "wpical")
endif()

if(WPILIB_WITH_TESTS)
    wpilib_add_test_catch2(wpical src/test/native/cpp)
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
            src/main/native/thirdparty/mrcal/generated
            src/main/native/thirdparty/mrcal_java/include
    )

    target_compile_definitions(wpical_test PUBLIC OPENCV_DISABLE_EIGEN_TENSOR_SUPPORT)
    target_compile_definitions(wpical_test PUBLIC PROJECT_ROOT_PATH="${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/assets")

    target_link_libraries(
        wpical_test
        apriltag
        wpimath
        ${OpenCV_LIBS}
        libglass
        wpigui
        wpiutil
        ${cholmod_target}
        ${suitesparse_target}
        Ceres::ceres
    )
endif()

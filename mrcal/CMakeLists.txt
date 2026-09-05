project(mrcal)

include(CompileWarnings)
include(AddTest)

find_package(OpenCV REQUIRED)
find_package(CHOLMOD CONFIG REQUIRED)
find_package(SuiteSparse_config CONFIG REQUIRED)

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

file(GLOB mrcal_native_src src/main/native/cpp/*.cpp)
file(
    GLOB_RECURSE mrcal_thirdparty_src
    src/main/native/thirdparty/libdogleg/src/*.c
    src/main/native/thirdparty/src/*.c
    src/main/native/thirdparty/src/*.cpp
)

add_library(mrcal STATIC ${mrcal_native_src} ${mrcal_thirdparty_src})
set_target_properties(mrcal PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET mrcal PROPERTY FOLDER "libraries")
target_compile_features(mrcal PUBLIC cxx_std_23)
wpilib_target_warnings(mrcal)

if(MSVC)
    set(mrcal_compile_flags
        /wd4098
        /wd4267
        /wd4068
        /wd4101
        /wd4200
        /wd4576
        /wd4715
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(mrcal_compile_flags
        -Wno-format-nonliteral
        -Wno-format-overflow
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
        -Wno-pedantic
        -Wno-return-type
        -Wno-uninitialized
    )
else()
    set(mrcal_compile_flags
        -Wno-format-nonliteral
        -Wno-format-overflow
        -Wno-unused-variable
        -Wno-unused-function
        -Wno-sign-compare
        -Wno-missing-field-initializers
        -Wno-pedantic
        -Wno-return-type
        -Wno-maybe-uninitialized
    )
endif()

# mrcal and libdogleg are vendored as-is, and the wrappers around them use their
# types directly, so the whole library needs the upstream warnings suppressed.
set_source_files_properties(
    ${mrcal_native_src}
    ${mrcal_thirdparty_src}
    PROPERTIES COMPILE_OPTIONS "${mrcal_compile_flags}"
)

target_include_directories(
    mrcal
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
)
target_include_directories(
    mrcal
    SYSTEM
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/generated>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/libdogleg/include>
)

# wpimath supplies Eigen, which mrcal-uncertainty.cpp needs.
target_link_libraries(
    mrcal
    PUBLIC wpimath opencv_core opencv_calib3d ${cholmod_target} ${suitesparse_target}
)

target_compile_definitions(mrcal PUBLIC OPENCV_DISABLE_EIGEN_TENSOR_SUPPORT)

if(WPILIB_WITH_TESTS)
    wpilib_add_test(mrcal src/test/native/cpp)
    target_link_libraries(mrcal_test mrcal)
endif()

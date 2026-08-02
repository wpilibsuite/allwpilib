project(apriltag)

include(CompileWarnings)
include(GenResources)

file(
    GLOB_RECURSE apriltaglib_src
    src/main/native/thirdparty/apriltag/src/*.c
    src/main/native/thirdparty/apriltag/src/*.cpp
)

# Disable apriltag C library warnings
if(MSVC)
    set_source_files_properties(
        ${apriltaglib_src}
        PROPERTIES COMPILE_FLAGS "/wd4005 /wd4018 /wd4244 /wd4267 /wd4996"
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set_source_files_properties(
        ${apriltaglib_src}
        PROPERTIES
            COMPILE_FLAGS
                "-Wno-format-nonliteral -Wno-gnu-zero-variadic-macro-arguments -Wno-uninitialized -Wno-sign-compare -Wno-type-limits"
    )
else()
    set_source_files_properties(
        ${apriltaglib_src}
        PROPERTIES
            COMPILE_FLAGS
                "-Wno-format-nonliteral -Wno-gnu-zero-variadic-macro-arguments -Wno-maybe-uninitialized -Wno-sign-compare -Wno-type-limits"
    )
endif()

generate_resources(
    src/main/native/resources/org/wpilib/vision/apriltag
    generated/main/cpp
    APRILTAG
    wpi::apriltag
    apriltag_resources_src
)

file(GLOB apriltag_jni_src src/main/native/cpp/jni/AprilTagJNI.cpp)

file(GLOB_RECURSE apriltag_native_src src/main/native/cpp/*.cpp)
list(REMOVE_ITEM apriltag_native_src ${apriltag_jni_src})

file(GLOB apriltag_protobuf_native_src src/generated/main/native/cpp/apriltag/protobuf/*.cpp)

add_library(
    apriltag
    ${apriltag_native_src}
    ${apriltag_protobuf_native_src}
    ${apriltag_resources_src}
    ${apriltaglib_src}
)
set_target_properties(apriltag PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET apriltag PROPERTY FOLDER "libraries")
target_compile_features(apriltag PUBLIC cxx_std_23)
wpilib_target_warnings(apriltag)

target_link_libraries(apriltag wpimath)

target_include_directories(
    apriltag
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/native/cpp>
        $<INSTALL_INTERFACE:${include_dest}/apriltag>
)

install(
    DIRECTORY src/generated/main/native/cpp/
    DESTINATION "${include_dest}/apriltag"
    FILES_MATCHING
    PATTERN "*.h"
)
install(
    DIRECTORY src/main/native/include/ src/main/native/thirdparty/apriltag/include/
    DESTINATION "${include_dest}/apriltag"
)
target_include_directories(
    apriltag
    SYSTEM
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/apriltag/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/apriltag/include/common>
)

install(TARGETS apriltag EXPORT apriltag)
export(TARGETS apriltag FILE apriltag.cmake NAMESPACE apriltag::)

configure_file(apriltag-config.cmake.in ${WPILIB_BINARY_DIR}/apriltag-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/apriltag-config.cmake DESTINATION share/apriltag)
install(EXPORT apriltag DESTINATION share/apriltag)

if(WPILIB_WITH_TESTS)
    wpilib_add_test_catch2(apriltag src/test/native/cpp)
    target_include_directories(apriltag_test PRIVATE src/test/native/include)
    target_link_libraries(apriltag_test apriltag)
endif()

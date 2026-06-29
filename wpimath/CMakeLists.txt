project(wpimath)

include(SubDirList)
include(CompileWarnings)
include(AddTest)

file(
    GLOB wpimath_jni_src
    src/main/native/cpp/jni/DAREJNI.cpp
    src/main/native/cpp/jni/EigenJNI.cpp
    src/main/native/cpp/jni/Exceptions.cpp
    src/main/native/cpp/jni/LinearSystemUtilJNI.cpp
    src/main/native/cpp/jni/Transform3dJNI.cpp
    src/main/native/cpp/jni/Twist3dJNI.cpp
    src/main/native/cpp/jni/autodiff/GradientJNI.cpp
    src/main/native/cpp/jni/autodiff/HessianJNI.cpp
    src/main/native/cpp/jni/autodiff/JacobianJNI.cpp
    src/main/native/cpp/jni/autodiff/VariableJNI.cpp
    src/main/native/cpp/jni/autodiff/VariableMatrixJNI.cpp
    src/main/native/cpp/jni/optimization/ProblemJNI.cpp
)

file(GLOB_RECURSE wpimath_native_src src/main/native/cpp/*.cpp)
list(REMOVE_ITEM wpimath_native_src ${wpimath_jni_src})

file(GLOB_RECURSE sleipnir_native_src src/main/native/thirdparty/sleipnir/src/*.cpp)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS FALSE)

file(GLOB_RECURSE wpimath_protobuf_native_src src/generated/main/native/cpp/wpimath/protobuf/*.cpp)

add_library(protobuf OBJECT ${wpimath_protobuf_native_src})
target_link_libraries(protobuf wpiutil)

add_library(wpimath ${wpimath_native_src} $<TARGET_OBJECTS:protobuf>)

if(MSVC)
    get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    set(CONFIG_SUFFIX "$<$<BOOL:${IS_MULTI_CONFIG}>:_$<CONFIG>>")
    file(
        GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/protobuf_objects${CONFIG_SUFFIX}.txt
        CONTENT $<LIST:JOIN,$<TARGET_OBJECTS:protobuf>,\n>
    )
    add_custom_command(
        TARGET wpimath
        PRE_LINK
        COMMAND
            cmake -E __create_def ${CMAKE_CURRENT_BINARY_DIR}/protobuf_exports${CONFIG_SUFFIX}.def
            ${CMAKE_CURRENT_BINARY_DIR}/protobuf_objects${CONFIG_SUFFIX}.txt
    )
    target_link_options(
        wpimath
        PRIVATE
            /DEF:$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/protobuf_exports${CONFIG_SUFFIX}.def>
    )
endif()

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set_target_properties(wpimath PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET wpimath PROPERTY FOLDER "libraries")
target_compile_definitions(wpimath PRIVATE WPILIB_EXPORTS SLEIPNIR_EXPORTS)

target_compile_features(wpimath PUBLIC cxx_std_23)
if(MSVC)
    target_compile_options(wpimath PUBLIC /utf-8 /bigobj)
endif()
wpilib_target_warnings(wpimath)
target_link_libraries(wpimath wpiutil)

if(NOT USE_SYSTEM_EIGEN)
    install(
        DIRECTORY src/main/native/thirdparty/eigen/include/
        DESTINATION "${include_dest}/wpimath"
    )
    target_include_directories(
        wpimath
        SYSTEM
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/eigen/include>
    )
else()
    find_package(Eigen3 CONFIG REQUIRED)
    target_link_libraries(wpimath Eigen3::Eigen)
endif()

install(DIRECTORY src/main/native/thirdparty/gcem/include/ DESTINATION "${include_dest}/wpimath")
target_include_directories(
    wpimath
    SYSTEM
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/gcem/include>
)

if(NOT USE_SYSTEM_SLEIPNIR)
    target_sources(wpimath PRIVATE ${sleipnir_native_src})
    install(
        DIRECTORY src/main/native/thirdparty/sleipnir/include/
        DESTINATION "${include_dest}/wpimath"
    )
    target_include_directories(
        wpimath
        SYSTEM
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/sleipnir/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/sleipnir/src>
    )
else()
    find_package(Sleipnir CONFIG REQUIRED)
    target_link_libraries(wpimath Sleipnir::Sleipnir)
    if(MSVC)
        get_target_property(Sleipnir_includes Sleipnir::Sleipnir INTERFACE_INCLUDE_DIRECTORIES)
        foreach(dir ${Sleipnir_includes})
            target_compile_options(wpimath PUBLIC /external:I "${dir}")
        endforeach()
        target_compile_options(wpimath PUBLIC /external:W0)
    endif()
endif()

install(
    DIRECTORY src/generated/main/native/cpp/
    DESTINATION "${include_dest}/wpimath"
    FILES_MATCHING
    PATTERN "*.h"
)
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/wpimath")
target_include_directories(
    wpimath
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/generated/main/native/cpp>
        $<INSTALL_INTERFACE:${include_dest}/wpimath>
)

install(TARGETS wpimath EXPORT wpimath)
export(TARGETS wpimath FILE wpimath.cmake NAMESPACE wpimath::)

configure_file(wpimath-config.cmake.in ${WPILIB_BINARY_DIR}/wpimath-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/wpimath-config.cmake DESTINATION share/wpimath)
install(EXPORT wpimath DESTINATION share/wpimath)

if(WITH_TESTS)
    wpilib_add_test_catch2(wpimath src/test/native/cpp)
    target_include_directories(wpimath_test PRIVATE src/test/native/include)
    target_link_libraries(wpimath_test wpimath)
endif()

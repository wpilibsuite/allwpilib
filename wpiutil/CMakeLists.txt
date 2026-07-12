project(wpiutil)

include(SubDirList)
include(GenResources)
include(CompileWarnings)
include(AddTest)

file(GLOB wpiutil_jni_src src/main/native/cpp/jni/WPIUtilJNI.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if(NOT MSVC AND NOT APPLE AND NOT ANDROID)
    find_library(ATOMIC NAMES atomic libatomic.so.1)
    if(ATOMIC)
        message(STATUS "Found libatomic: ${ATOMIC}")
    else()
        message(STATUS "libatomic not found. If build fails, install libatomic")
    endif()
endif()

generate_resources(src/main/native/resources generated/main/cpp WPI wpi wpiutil_resources_src)

file(
    GLOB_RECURSE wpiutil_native_src
    src/main/native/cpp/*.cpp
    src/main/native/thirdparty/debugging/src/*.cpp
    src/main/native/thirdparty/double-conversion/src/*.cpp
    src/main/native/thirdparty/json/src/*.cpp
    src/main/native/thirdparty/llvm/cpp/*.cpp
    src/main/native/thirdparty/mpack/src/*.cpp
    src/main/native/thirdparty/nanopb/src/*.cpp
    src/main/native/thirdparty/upb/src/*.c
)
list(REMOVE_ITEM wpiutil_native_src ${wpiutil_jni_src})
file(GLOB_RECURSE wpiutil_unix_src src/main/native/unix/*.cpp)
file(GLOB_RECURSE wpiutil_windows_src src/main/native/windows/*.cpp)

add_library(wpiutil ${wpiutil_native_src} ${wpiutil_resources_src})
set_target_properties(wpiutil PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET wpiutil PROPERTY FOLDER "libraries")

target_compile_features(wpiutil PUBLIC cxx_std_23)
if(MSVC)
    target_compile_options(
        wpiutil
        PUBLIC /permissive- /Zc:preprocessor /Zc:__cplusplus /Zc:throwingNew /MP /bigobj /utf-8
    )
    target_compile_definitions(wpiutil PRIVATE -D_CRT_SECURE_NO_WARNINGS)
endif()
wpilib_target_warnings(wpiutil)
target_link_libraries(wpiutil Threads::Threads ${CMAKE_DL_LIBS})

if(ATOMIC)
    target_link_libraries(wpiutil ${ATOMIC})
endif()

if(MSVC)
    target_sources(wpiutil PRIVATE ${wpiutil_windows_src})
else()
    target_sources(wpiutil PRIVATE ${wpiutil_unix_src})
endif()

install(
    DIRECTORY
        src/main/native/include/
        src/main/native/thirdparty/argparse/include/
        src/main/native/thirdparty/debugging/include/
        src/main/native/thirdparty/double-conversion/include/
        src/main/native/thirdparty/json/include/
        src/main/native/thirdparty/llvm/include/
        src/main/native/thirdparty/mpack/include/
        src/main/native/thirdparty/nanopb/include/
        src/main/native/thirdparty/sigslot/include/
        src/main/native/thirdparty/upb/include/
    DESTINATION "${include_dest}/wpiutil"
)
target_include_directories(
    wpiutil
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/argparse/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/debugging/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/double-conversion/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/json/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/llvm/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/mpack/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/nanopb/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/sigslot/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/upb/include>
        $<INSTALL_INTERFACE:${include_dest}/wpiutil>
)

install(TARGETS wpiutil EXPORT wpiutil)
export(TARGETS wpiutil FILE wpiutil.cmake NAMESPACE wpiutil::)

configure_file(wpiutil-config.cmake.in ${WPILIB_BINARY_DIR}/wpiutil-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/wpiutil-config.cmake DESTINATION share/wpiutil)
install(EXPORT wpiutil DESTINATION share/wpiutil)

add_executable(wpiutildev src/dev/native/cpp/main.cpp)
wpilib_target_warnings(wpiutildev)
target_link_libraries(wpiutildev wpiutil)

subdir_list(wpiutil_examples "${CMAKE_CURRENT_SOURCE_DIR}/examples")
foreach(example ${wpiutil_examples})
    file(GLOB wpiutil_example_src examples/${example}/*.cpp)
    if(wpiutil_example_src)
        add_executable(wpiutil_${example} ${wpiutil_example_src})
        wpilib_target_warnings(wpiutil_${example})
        target_link_libraries(wpiutil_${example} wpiutil)
        set_property(TARGET wpiutil_${example} PROPERTY FOLDER "examples")
    endif()
endforeach()

if(WPILIB_WITH_TESTS)
    file(GLOB_RECURSE wpiutil_testlib_src src/test/native/include/*.h)
    add_library(wpiutil_testlib INTERFACE ${wpiutil_test_src})
    target_include_directories(wpiutil_testlib INTERFACE src/test/native/include)

    # Not using wpilib_add_test until we migrate more tests
    file(GLOB_RECURSE test_src src/test/native/cpp/*.cpp)
    file(GLOB catch2_test_src src/test/native/cpp/catch2main.cpp src/test/native/cpp/TestCatch2.cpp)
    list(REMOVE_ITEM test_src ${catch2_test_src})
    add_executable(wpiutil_test ${test_src})
    set_property(TARGET wpiutil_test PROPERTY FOLDER "tests")
    wpilib_target_warnings(wpiutil_test)
    if(BUILD_SHARED_LIBS)
        target_compile_definitions(wpiutil_test PRIVATE -DGTEST_LINKED_AS_SHARED_LIBRARY)
    endif()
    if(MSVC)
        target_compile_options(wpiutil_test PRIVATE /wd4101 /wd4251)
    endif()
    add_test(NAME wpiutil COMMAND wpiutil_test)
    target_include_directories(wpiutil_test PRIVATE src/generated/test/native/cpp)
    file(GLOB_RECURSE wpiutil_nanopb_test_src src/generated/test/native/cpp/*.cpp)
    target_sources(wpiutil_test PRIVATE ${wpiutil_nanopb_test_src})
    target_link_libraries(wpiutil_test wpiutil googletest wpiutil_testlib)

    wpilib_add_test_catch2(wpiutil_catch2 ${catch2_test_src})
    target_link_libraries(wpiutil_catch2_test wpiutil wpiutil_testlib)
    if(MSVC)
        target_compile_options(wpiutil_test PRIVATE /utf-8)
    endif()
endif()

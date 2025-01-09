project(libdatalog)

include(CompileWarnings)

file(
    GLOB libdatalog_src
    src/main/native/cpp/*.cpp
)

file(
    GLOB libdatalog_jni_src
    src/main/native/cpp/jni/DataLogJNI.cpp
)

add_library(libdatalog ${libdatalog_src})
set_target_properties(libdatalog PROPERTIES DEBUG_POSTFIX "d")

target_compile_features(libdatalog PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(
        libdatalog
        PUBLIC /permissive- /Zc:preprocessor /Zc:__cplusplus /Zc:throwingNew /MP /bigobj /utf-8
    )
    target_compile_definitions(libdatalog PRIVATE -D_CRT_SECURE_NO_WARNINGS)
endif()
wpilib_target_warnings(libdatalog)

target_include_directories(
    libdatalog
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
)

target_link_libraries(libdatalog PRIVATE wpiutil)

subdir_list(libdatalog_examples "${CMAKE_CURRENT_SOURCE_DIR}/examples")
foreach(example ${libdatalog_examples})
    file(GLOB libdatalog_example_src examples/${example}/*.cpp)
    if(libdatalog_example_src)
        add_executable(libdatalog_${example} ${libdatalog_example_src})
        wpilib_target_warnings(libdatalog_${example})
        target_link_libraries(libdatalog_${example} libdatalog)
        set_property(TARGET libdatalog_${example} PROPERTY FOLDER "examples")
    endif()
endforeach()

add_library(libdatalogjni ${libdatalog_jni_src})
wpilib_target_warnings(libdatalogjni)
target_link_libraries(libdatalogjni PUBLIC libdatalog)

if(WITH_TESTS)
    file(GLOB_RECURSE libdatalog_testlib_src src/test/native/include/*.h)
    add_library(libdatalog_testlib INTERFACE ${libdatalog_test_src})
    target_include_directories(libdatalog_testlib INTERFACE src/test/native/include)

    wpilib_add_test(libdatalog src/test/native/cpp)
    target_include_directories(libdatalog_test PRIVATE src/generated/test/native/cpp)
    target_link_libraries(libdatalog_test libdatalog googletest libdatalog_testlib)
    if(MSVC)
        target_compile_options(lobdatalog_test PRIVATE /utf-8)
    endif()
endif()


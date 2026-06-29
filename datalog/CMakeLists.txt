project(datalog)

include(CompileWarnings)

file(GLOB datalog_native_src src/main/native/cpp/*.cpp)

add_library(datalog ${datalog_native_src})
set_target_properties(datalog PROPERTIES DEBUG_POSTFIX "d")

target_compile_features(datalog PUBLIC cxx_std_23)
wpilib_target_warnings(datalog)

target_include_directories(
    datalog
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
)

target_link_libraries(datalog PUBLIC wpiutil)

subdir_list(datalog_examples "${CMAKE_CURRENT_SOURCE_DIR}/examples")
foreach(example ${datalog_examples})
    file(GLOB datalog_example_src examples/${example}/*.cpp)
    if(datalog_example_src)
        add_executable(datalog_${example} ${datalog_example_src})
        wpilib_target_warnings(datalog_${example})
        target_link_libraries(datalog_${example} datalog wpiutil)
        set_property(TARGET datalog_${example} PROPERTY FOLDER "examples")
    endif()
endforeach()

install(TARGETS datalog EXPORT datalog)
export(TARGETS datalog FILE datalog.cmake NAMESPACE datalog::)

configure_file(datalog-config.cmake.in ${WPILIB_BINARY_DIR}/datalog-config.cmake)
install(FILES ${WPILIB_BINARY_DIR}/datalog-config.cmake DESTINATION share/datalog)
install(EXPORT datalog DESTINATION share/datalog)

if(WITH_TESTS)
    file(GLOB_RECURSE datalog_testlib_src src/test/native/include/*.h)
    add_library(datalog_testlib INTERFACE ${datalog_test_src})
    target_include_directories(datalog_testlib INTERFACE src/test/native/include)

    wpilib_add_test_catch2(datalog src/test/native/cpp)
    target_link_libraries(datalog_test datalog datalog_testlib wpiutil)
    if(MSVC)
        target_compile_options(datalog_test PRIVATE /utf-8)
    endif()
endif()

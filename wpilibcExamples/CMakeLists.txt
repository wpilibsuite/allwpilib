project(wpilibcExamples)

include(AddTest)
include(SubDirList)

subdir_list(TEMPLATES ${CMAKE_SOURCE_DIR}/wpilibcExamples/src/main/cpp/templates)
subdir_list(EXAMPLES ${CMAKE_SOURCE_DIR}/wpilibcExamples/src/main/cpp/examples)

add_custom_target(wpilibcExamples)
add_custom_target(wpilibcExamples_test)

foreach(example ${EXAMPLES})
    file(
        GLOB_RECURSE sources
        src/main/cpp/examples/${example}/cpp/*.cpp
        src/main/cpp/examples/${example}/c/*.c
    )
    add_executable(${example} ${sources})
    wpilib_target_warnings(${example})
    target_include_directories(${example} PUBLIC src/main/cpp/examples/${example}/include)
    target_link_libraries(
        ${example}
        apriltag
        wpilibc
        wpilibNewCommands
        romiVendordep
        xrpVendordep
    )
    add_dependencies(wpilibcExamples ${example})

    if(WITH_TESTS AND EXISTS ${CMAKE_SOURCE_DIR}/wpilibcExamples/src/test/cpp/examples/${example})
        wpilib_add_test(Example_${example} src/test/cpp/examples/${example}/cpp)
        target_sources(Example_${example}_test PRIVATE ${sources})
        target_include_directories(
            Example_${example}_test
            PRIVATE
                src/main/cpp/examples/${example}/include
                src/test/cpp/examples/${example}/include
        )
        target_compile_definitions(Example_${example}_test PUBLIC RUNNING_FRC_TESTS)
        target_link_libraries(
            Example_${example}_test
            apriltag
            wpilibc
            wpilibNewCommands
            romiVendordep
            xrpVendordep
            googletest
        )
        add_dependencies(wpilibcExamples_test Example_${example}_test)
    endif()
endforeach()

foreach(template ${TEMPLATES})
    file(
        GLOB_RECURSE sources
        src/main/cpp/templates/${template}/cpp/*.cpp
        src/main/cpp/templates/${template}/c/*.c
    )
    add_executable(${template} ${sources})
    wpilib_target_warnings(${template})
    target_include_directories(${template} PUBLIC src/main/cpp/templates/${template}/include)
    target_link_libraries(${template} wpilibc wpilibNewCommands romiVendordep xrpVendordep)
    add_dependencies(wpilibcExamples ${template})
endforeach()

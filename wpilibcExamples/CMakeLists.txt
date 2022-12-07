project(wpilibcExamples)

include(AddTest)
include(SubDirList)

SUBDIR_LIST(TEMPLATES ${CMAKE_SOURCE_DIR}/wpilibcExamples/src/main/cpp/templates)
SUBDIR_LIST(EXAMPLES  ${CMAKE_SOURCE_DIR}/wpilibcExamples/src/main/cpp/examples)

foreach(example ${EXAMPLES})
  file(GLOB_RECURSE sources src/main/cpp/examples/${example}/cpp/*.cpp
                            src/main/cpp/examples/${example}/c/*.c)
  add_executable(${example} ${sources})
  target_include_directories(${example} PUBLIC src/main/cpp/examples/${example}/include)
  target_link_libraries(${example} wpilibc wpilibNewCommands)

  if (WITH_TESTS AND EXISTS ${CMAKE_SOURCE_DIR}/wpilibcExamples/src/test/cpp/examples/${example})
    wpilib_add_test(${example} src/test/cpp/examples/${example}/cpp)
    target_sources(${example}_test PRIVATE ${sources})
    target_include_directories(${example}_test PRIVATE
                               src/main/cpp/examples/${example}/include
                               src/test/cpp/examples/${example}/include)
    target_compile_definitions(${example}_test PUBLIC RUNNING_FRC_TESTS)
    target_link_libraries(${example}_test wpilibc wpilibNewCommands gmock_main)
  endif()
endforeach()

foreach(template ${TEMPLATES})
  file(GLOB_RECURSE sources src/main/cpp/templates/${template}/cpp/*.cpp
                            src/main/cpp/templates/${template}/c/*.c)
  add_executable(${template} ${sources})
  target_include_directories(${template} PUBLIC src/main/cpp/templates/${template}/include)
  target_link_libraries(${template} wpilibc wpilibNewCommands)
endforeach()

project(wpilibNewCommands)

include(SubDirList)
include(CompileWarnings)
include(AddTest)

if (WITH_JAVA)
  find_package(Java REQUIRED)
  include(UseJava)
  set(CMAKE_JAVA_COMPILE_FLAGS "-encoding" "UTF8" "-Xlint:unchecked")

  file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
  add_jar(wpilibNewCommands_jar ${JAVA_SOURCES} INCLUDE_JARS hal_jar ntcore_jar cscore_jar cameraserver_jar wpimath_jar wpiutil_jar wpilibj_jar OUTPUT_NAME wpilibNewCommands)

  get_property(WPILIBNEWCOMMANDS_JAR_FILE TARGET wpilibNewCommands_jar PROPERTY JAR_FILE)
  install(FILES ${WPILIBNEWCOMMANDS_JAR_FILE} DESTINATION "${java_lib_dest}")

  set_property(TARGET wpilibNewCommands_jar PROPERTY FOLDER "java")

  if (WITH_FLAT_INSTALL)
      set (wpilibNewCommands_config_dir ${wpilib_dest})
  else()
      set (wpilibNewCommands_config_dir share/wpilibNewCommands)
  endif()

  install(FILES wpilibNewCommands-config.cmake DESTINATION ${wpilibNewCommands_config_dir})
endif()

file(GLOB_RECURSE wpilibNewCommands_native_src src/main/native/cpp/*.cpp)
add_library(wpilibNewCommands ${wpilibNewCommands_native_src})
set_target_properties(wpilibNewCommands PROPERTIES DEBUG_POSTFIX "d")
set_property(TARGET wpilibNewCommands PROPERTY FOLDER "libraries")

target_compile_features(wpilibNewCommands PUBLIC cxx_std_20)
wpilib_target_warnings(wpilibNewCommands)
target_link_libraries(wpilibNewCommands wpilibc)

target_include_directories(wpilibNewCommands PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
                            $<INSTALL_INTERFACE:${include_dest}/wpilibNewCommands>)

install(TARGETS wpilibNewCommands EXPORT wpilibNewCommands DESTINATION "${main_lib_dest}")
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/wpilibNewCommands")

if (MSVC OR FLAT_INSTALL_WPILIB)
     set(wpilibNewCommands_config_dir ${wpilib_dest})
 else()
     set(wpilibNewCommands_config_dir share/wpilibNewCommands)
 endif()

 configure_file(wpilibNewCommands-config.cmake.in ${WPILIB_BINARY_DIR}/wpilibNewCommands-config.cmake)
 install(FILES ${WPILIB_BINARY_DIR}/wpilibNewCommands-config.cmake DESTINATION ${wpilibNewCommands_config_dir})
 install(EXPORT wpilibNewCommands DESTINATION ${wpilibNewCommands_config_dir})

 if (WITH_TESTS)
     wpilib_add_test(wpilibNewCommands src/test/native/cpp)
     target_include_directories(wpilibNewCommands_test PRIVATE src/test/native/include)
     target_link_libraries(wpilibNewCommands_test wpilibNewCommands gmock_main)
 endif()

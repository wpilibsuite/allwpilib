project(wpilibOldCommands)

include(SubDirList)
include(CompileWarnings)
include(AddTest)

if (WITH_JAVA)
  find_package(Java REQUIRED)
  include(UseJava)
  set(CMAKE_JAVA_COMPILE_FLAGS "-Xlint:unchecked")

  file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
  add_jar(wpilibOldCommands_jar ${JAVA_SOURCES} INCLUDE_JARS hal_jar ntcore_jar cscore_jar cameraserver_jar wpimath_jar wpiutil_jar wpilibj_jar OUTPUT_NAME wpilibOldCommands)

  get_property(WPIlIBOLDCOMMANDS_JAR_FILE TARGET wpilibOldCommands_jar PROPERTY JAR_FILE)
  install(FILES ${WPIlIBOLDCOMMANDS_JAR_FILE} DESTINATION "${java_lib_dest}")

  set_property(TARGET wpilibOldCommands_jar PROPERTY FOLDER "java")

  if (WITH_FLAT_INSTALL)
      set (wpilibOldCommands_config_dir ${wpilib_dest})
  else()
      set (wpilibOldCommands_config_dir share/wpilibOldCommands)
  endif()

  install(FILES wpilibOldCommands-config.cmake DESTINATION ${wpilibOldCommands_config_dir})
endif()

file(GLOB_RECURSE wpilibOldCommands_native_src src/main/native/cpp/*.cpp)
add_library(wpilibOldCommands ${wpilibOldCommands_native_src})
set_target_properties(wpilibOldCommands PROPERTIES DEBUG_POSTFIX "d")
set_property(TARGET wpilibOldCommands PROPERTY FOLDER "libraries")

target_compile_features(wpilibOldCommands PUBLIC cxx_std_17)
wpilib_target_warnings(wpilibOldCommands)
target_link_libraries(wpilibOldCommands wpilibc)

target_include_directories(wpilibOldCommands PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
                            $<INSTALL_INTERFACE:${include_dest}/wpilibOldCommands>)

install(TARGETS wpilibOldCommands EXPORT wpilibOldCommands DESTINATION "${main_lib_dest}")
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/wpilibOldCommands")

if (MSVC OR FLAT_INSTALL_WPILIB)
     set(wpilibOldCommands_config_dir ${wpilib_dest})
 else()
     set(wpilibOldCommands_config_dir share/wpilibOldCommands)
 endif()

 configure_file(wpilibOldCommands-config.cmake.in ${WPILIB_BINARY_DIR}/wpilibOldCommands-config.cmake)
 install(FILES ${WPILIB_BINARY_DIR}/wpilibOldCommands-config.cmake DESTINATION ${wpilibOldCommands_config_dir})
 install(EXPORT wpilibOldCommands DESTINATION ${wpilibOldCommands_config_dir})

 if (WITH_TESTS)
     wpilib_add_test(wpilibOldCommands src/test/native/cpp)
     target_include_directories(wpilibOldCommands_test PRIVATE src/test/native/include)
     target_link_libraries(wpilibOldCommands_test wpilibOldCommands gmock_main)
 endif()

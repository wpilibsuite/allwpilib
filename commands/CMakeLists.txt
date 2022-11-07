project(commands)

include(SubDirList)
include(CompileWarnings)
include(AddTest)

if (WITH_JAVA)
  find_package(Java REQUIRED)
  include(UseJava)
  set(CMAKE_JAVA_COMPILE_FLAGS "-encoding" "UTF8" "-Xlint:unchecked")

  file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
  add_jar(commands_jar ${JAVA_SOURCES} INCLUDE_JARS hal_jar ntcore_jar cscore_jar cameraserver_jar wpimath_jar wpiutil_jar wpilibj_jar OUTPUT_NAME commands)

  get_property(COMMANDS_JAR_FILE TARGET commands_jar PROPERTY JAR_FILE)
  install(FILES ${COMMANDS_JAR_FILE} DESTINATION "${java_lib_dest}")

  set_property(TARGET commands_jar PROPERTY FOLDER "java")

  if (WITH_FLAT_INSTALL)
      set (commands_config_dir ${wpilib_dest})
  else()
      set (commands_config_dir share/commands)
  endif()

  install(FILES commands-config.cmake DESTINATION ${commands_config_dir})
endif()

file(GLOB_RECURSE commands_native_src src/main/native/cpp/*.cpp)
add_library(commands ${commands_native_src})
set_target_properties(commands PROPERTIES DEBUG_POSTFIX "d")
set_property(TARGET commands PROPERTY FOLDER "libraries")

target_compile_features(commands PUBLIC cxx_std_20)
wpilib_target_warnings(commands)
target_link_libraries(commands wpilibc)

target_include_directories(commands PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
                            $<INSTALL_INTERFACE:${include_dest}/commands>)

install(TARGETS commands EXPORT commands DESTINATION "${main_lib_dest}")
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/commands")

if (MSVC OR FLAT_INSTALL_WPILIB)
     set(commands_config_dir ${wpilib_dest})
 else()
     set(commands_config_dir share/commands)
 endif()

 configure_file(commands-config.cmake.in ${WPILIB_BINARY_DIR}/commands-config.cmake)
 install(FILES ${WPILIB_BINARY_DIR}/commands-config.cmake DESTINATION ${commands_config_dir})
 install(EXPORT commands DESTINATION ${commands_config_dir})

 if (WITH_TESTS)
     wpilib_add_test(commands src/test/native/cpp)
     target_include_directories(commands_test PRIVATE src/test/native/include)
     target_link_libraries(commands_test commands gmock_main)
 endif()

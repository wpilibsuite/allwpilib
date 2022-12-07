project(wpimath)

include(SubDirList)
include(CompileWarnings)
include(AddTest)

file(GLOB wpimath_jni_src src/main/native/cpp/jni/WPIMathJNI.cpp)

# Java bindings
if (WITH_JAVA)
  find_package(Java REQUIRED)
  find_package(JNI REQUIRED)
  include(UseJava)
  set(CMAKE_JAVA_COMPILE_FLAGS "-encoding" "UTF8" "-Xlint:unchecked")

  if(NOT EXISTS "${WPILIB_BINARY_DIR}/wpimath/thirdparty/ejml/ejml-simple-0.41.jar")
      set(BASE_URL "https://search.maven.org/remotecontent?filepath=")
      set(JAR_ROOT "${WPILIB_BINARY_DIR}/wpimath/thirdparty/ejml")

      message(STATUS "Downloading EJML jarfiles...")

      file(DOWNLOAD "${BASE_URL}org/ejml/ejml-cdense/0.41/ejml-cdense-0.41.jar"
          "${JAR_ROOT}/ejml-cdense-0.41.jar")
      file(DOWNLOAD "${BASE_URL}org/ejml/ejml-core/0.41/ejml-core-0.41.jar"
          "${JAR_ROOT}/ejml-core-0.41.jar")
      file(DOWNLOAD "${BASE_URL}org/ejml/ejml-ddense/0.41/ejml-ddense-0.41.jar"
          "${JAR_ROOT}/ejml-ddense-0.41.jar")
      file(DOWNLOAD "${BASE_URL}org/ejml/ejml-dsparse/0.41/ejml-dsparse-0.41.jar"
          "${JAR_ROOT}/ejml-dsparse-0.41.jar")
      file(DOWNLOAD "${BASE_URL}org/ejml/ejml-fdense/0.41/ejml-fdense-0.41.jar"
          "${JAR_ROOT}/ejml-fdense-0.41.jar")
      file(DOWNLOAD "${BASE_URL}org/ejml/ejml-simple/0.41/ejml-simple-0.41.jar"
          "${JAR_ROOT}/ejml-simple-0.41.jar")
      file(DOWNLOAD "${BASE_URL}org/ejml/ejml-zdense/0.41/ejml-zdense-0.41.jar"
          "${JAR_ROOT}/ejml-zdense-0.41.jar")

      message(STATUS "All files downloaded.")
  endif()

  file(GLOB EJML_JARS "${WPILIB_BINARY_DIR}/wpimath/thirdparty/ejml/*.jar")
  file(GLOB JACKSON_JARS "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson/*.jar")

  set(CMAKE_JAVA_INCLUDE_PATH wpimath.jar ${EJML_JARS} ${JACKSON_JARS})

  execute_process(COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/generate_numbers.py ${WPILIB_BINARY_DIR}/wpimath RESULT_VARIABLE generateResult)
  if(NOT (generateResult EQUAL "0"))
    # Try python
    execute_process(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/generate_numbers.py ${WPILIB_BINARY_DIR}/wpimath RESULT_VARIABLE generateResult)
    if(NOT (generateResult EQUAL "0"))
      message(FATAL_ERROR "python and python3 generate_numbers.py failed")
    endif()
  endif()

  set(CMAKE_JNI_TARGET true)

  file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java ${WPILIB_BINARY_DIR}/wpimath/generated/*.java)

  if(${CMAKE_VERSION} VERSION_LESS "3.11.0")
    set(CMAKE_JAVA_COMPILE_FLAGS "-h" "${CMAKE_CURRENT_BINARY_DIR}/jniheaders")
    add_jar(wpimath_jar ${JAVA_SOURCES} INCLUDE_JARS ${EJML_JARS} wpiutil_jar OUTPUT_NAME wpimath)
  else()
    add_jar(wpimath_jar ${JAVA_SOURCES} INCLUDE_JARS ${EJML_JARS} wpiutil_jar OUTPUT_NAME wpimath GENERATE_NATIVE_HEADERS wpimath_jni_headers)
  endif()

  get_property(WPIMATH_JAR_FILE TARGET wpimath_jar PROPERTY JAR_FILE)
  install(FILES ${WPIMATH_JAR_FILE} DESTINATION "${java_lib_dest}")

  set_property(TARGET wpimath_jar PROPERTY FOLDER "java")

  add_library(wpimathjni ${wpimath_jni_src})
  wpilib_target_warnings(wpimathjni)
  target_link_libraries(wpimathjni PUBLIC wpimath)

  set_property(TARGET wpimathjni PROPERTY FOLDER "libraries")

  if(${CMAKE_VERSION} VERSION_LESS "3.11.0")
    target_include_directories(wpimathjni PRIVATE ${JNI_INCLUDE_DIRS})
    target_include_directories(wpimathjni PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/jniheaders")
  else()
    target_link_libraries(wpimathjni PRIVATE wpimath_jni_headers)
  endif()
  add_dependencies(wpimathjni wpimath_jar)

  if (MSVC)
    install(TARGETS wpimathjni RUNTIME DESTINATION "${jni_lib_dest}" COMPONENT Runtime)
  endif()

  install(TARGETS wpimathjni EXPORT wpimathjni DESTINATION "${main_lib_dest}")

endif()

file(GLOB_RECURSE wpimath_native_src src/main/native/cpp/*.cpp
                                     src/main/native/thirdparty/drake/src/*.cpp)
list(REMOVE_ITEM wpimath_native_src ${wpimath_jni_src})

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS FALSE)
add_library(wpimath ${wpimath_native_src})
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set_target_properties(wpimath PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET wpimath PROPERTY FOLDER "libraries")
target_compile_definitions(wpimath PRIVATE WPILIB_EXPORTS)

target_compile_features(wpimath PUBLIC cxx_std_20)
if (MSVC)
    target_compile_options(wpimath PUBLIC /bigobj)
endif()
wpilib_target_warnings(wpimath)
target_link_libraries(wpimath wpiutil)

if (NOT USE_SYSTEM_EIGEN)
    install(DIRECTORY src/main/native/thirdparty/eigen/include/ DESTINATION "${include_dest}/wpimath")
    target_include_directories(wpimath SYSTEM PUBLIC
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/eigen/include>
                               $<INSTALL_INTERFACE:${include_dest}/wpimath>)
else()
    find_package(Eigen3 CONFIG REQUIRED)
    target_link_libraries (wpimath Eigen3::Eigen)
endif()

install(DIRECTORY src/main/native/thirdparty/drake/include/ DESTINATION "${include_dest}/wpimath")
target_include_directories(wpimath SYSTEM PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/drake/include>)

install(DIRECTORY src/main/native/thirdparty/gcem/include/ DESTINATION "${include_dest}/wpimath")
target_include_directories(wpimath SYSTEM PUBLIC
                          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/thirdparty/gcem/include>)

install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/wpimath")
target_include_directories(wpimath PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
                            $<INSTALL_INTERFACE:${include_dest}/wpimath>)

install(TARGETS wpimath EXPORT wpimath DESTINATION "${main_lib_dest}")

if (WITH_JAVA AND MSVC)
    install(TARGETS wpimath RUNTIME DESTINATION "${jni_lib_dest}" COMPONENT Runtime)
endif()

if (WITH_FLAT_INSTALL)
    set (wpimath_config_dir ${wpilib_dest})
else()
    set (wpimath_config_dir share/wpimath)
endif()

configure_file(wpimath-config.cmake.in ${WPILIB_BINARY_DIR}/wpimath-config.cmake )
install(FILES ${WPILIB_BINARY_DIR}/wpimath-config.cmake DESTINATION ${wpimath_config_dir})
install(EXPORT wpimath DESTINATION ${wpimath_config_dir})

if (WITH_TESTS)
    wpilib_add_test(wpimath src/test/native/cpp)
    target_include_directories(wpimath_test PRIVATE src/test/native/include)
    target_link_libraries(wpimath_test wpimath gmock_main)
endif()

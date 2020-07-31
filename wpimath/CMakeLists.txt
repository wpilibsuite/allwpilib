# Download and unpack wpimath at configure time
configure_file(CMakeLists.txt.in wpimath-download/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/wpimath-download )
if(result)
  message(FATAL_ERROR "CMake step for wpimath failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/wpimath-download )
if(result)
  message(FATAL_ERROR "Build step for wpimath failed: ${result}")
endif()

# Add wpimath directly to our build.
set(wpimath_srcdir ${CMAKE_CURRENT_BINARY_DIR}/wpimath-src)
file(GLOB wpimath_jni_src ${wpimath_srcdir}/src/main/native/cpp/drake/jni/*.cpp)
file(GLOB_RECURSE wpimath_sources ${wpimath_srcdir}/src/main/native/cpp/*.cpp)
list(REMOVE_ITEM wpimath_sources ${wpimath_jni_src})
add_library(wpimath STATIC ${wpimath_sources})
target_include_directories(wpimath PUBLIC "$<BUILD_INTERFACE:${wpimath_srcdir}/src/main/native/include>" "$<INSTALL_INTERFACE:${include_dest}/wpimath>")

set_property(TARGET wpimath PROPERTY POSITION_INDEPENDENT_CODE ON)

install(TARGETS wpimath EXPORT wpimath DESTINATION "${main_lib_dest}")
install(DIRECTORY ${wpimath_srcdir}/src/main/native/include/ DESTINATION "${include_dest}/wpimath")

if (MSVC OR FLAT_INSTALL_WPILIB)
    set (wpimath_config_dir ${wpilib_dest})
else()
    set (wpimath_config_dir share/wpimath)
endif()

configure_file(wpimath-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/wpimath-config.cmake )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/wpimath-config.cmake DESTINATION ${wpimath_config_dir})
install(EXPORT wpimath DESTINATION ${wpimath_config_dir})


if (NOT WITHOUT_JAVA)
  find_package(Java REQUIRED)
  find_package(JNI REQUIRED)
  include(UseJava)
  set(CMAKE_JAVA_COMPILE_FLAGS "-Xlint:unchecked")

  if (NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/ejml/ejml-simple-0.38.jar")
    set(BASE_URL "https://search.maven.org/remotecontent?filepath=")
    set(JAR_ROOT "${CMAKE_CURRENT_BINARY_DIR}/ejml")

    message(STATUS "Downloading EJML jarfiles...")

    file(DOWNLOAD "${BASE_URL}org/ejml/ejml-cdense/0.38/ejml-cdense-0.38.jar"
        "${JAR_ROOT}/ejml-cdense-0.38.jar")
    file(DOWNLOAD "${BASE_URL}org/ejml/ejml-core/0.38/ejml-core-0.38.jar"
        "${JAR_ROOT}/ejml-core-0.38.jar")
    file(DOWNLOAD "${BASE_URL}org/ejml/ejml-ddense/0.38/ejml-ddense-0.38.jar"
        "${JAR_ROOT}/ejml-ddense-0.38.jar")
    file(DOWNLOAD "${BASE_URL}org/ejml/ejml-dsparse/0.38/ejml-dsparse-0.38.jar"
        "${JAR_ROOT}/ejml-dsparse-0.38.jar")
    file(DOWNLOAD "${BASE_URL}org/ejml/ejml-fdense/0.38/ejml-fdense-0.38.jar"
        "${JAR_ROOT}/ejml-fdense-0.38.jar")
    file(DOWNLOAD "${BASE_URL}org/ejml/ejml-simple/0.38/ejml-simple-0.38.jar"
        "${JAR_ROOT}/ejml-simple-0.38.jar")
    file(DOWNLOAD "${BASE_URL}org/ejml/ejml-zdense/0.38/ejml-zdense-0.38.jar"
        "${JAR_ROOT}/ejml-zdense-0.38.jar")

    message(STATUS "All files downloaded.")
  endif()

  file(GLOB EJML_JARS ${CMAKE_CURRENT_BINARY_DIR}/ejml/*.jar)

  set(CMAKE_JAVA_INCLUDE_PATH wpimath.jar ${EJML_JARS})

  execute_process(COMMAND python3 ${wpimath_srcdir}/generate_numbers.py ${CMAKE_CURRENT_BINARY_DIR} RESULT_VARIABLE generateResult)
  if(NOT (generateResult EQUAL "0"))
    # Try python
    execute_process(COMMAND python ${wpimath_srcdir}/generate_numbers.py ${CMAKE_CURRENT_BINARY_DIR} RESULT_VARIABLE generateResult)
    if(NOT (generateResult EQUAL "0"))
      message(FATAL_ERROR "python and python3 generate_numbers.py failed")
    endif()
  endif()

  set(CMAKE_JNI_TARGET true)

  file(GLOB_RECURSE JAVA_SOURCES ${wpimath_srcdir}/src/main/java/*.java ${CMAKE_CURRENT_BINARY_DIR}/generated/*.java)

  if(${CMAKE_VERSION} VERSION_LESS "3.11.0")
    set(CMAKE_JAVA_COMPILE_FLAGS "-h" "${CMAKE_CURRENT_BINARY_DIR}/jniheaders")
    add_jar(wpimath_jar ${JAVA_SOURCES} INCLUDE_JARS ${EJML_JARS} OUTPUT_NAME wpimath)
  else()
    add_jar(wpimath_jar ${JAVA_SOURCES} INCLUDE_JARS ${EJML_JARS} OUTPUT_NAME wpimath GENERATE_NATIVE_HEADERS wpimath_jni_headers)
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

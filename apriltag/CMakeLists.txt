project(apriltag)

include(CompileWarnings)
include(GenResources)
include(FetchContent)

FetchContent_Declare(
    apriltaglib
    GIT_REPOSITORY  https://github.com/wpilibsuite/apriltag.git
    GIT_TAG         ad31e33d20f9782b7239cb15cde57c56c91383ad
)

# Don't use apriltag's CMakeLists.txt due to conflicting naming and JNI
FetchContent_GetProperties(apriltaglib)
if(NOT apriltaglib_POPULATED)
    FetchContent_Populate(apriltaglib)
endif()

aux_source_directory(${apriltaglib_SOURCE_DIR}/common APRILTAGLIB_COMMON_SRC)
file(GLOB TAG_FILES ${apriltaglib_SOURCE_DIR}/tag*.c)
set(APRILTAGLIB_SRCS ${apriltaglib_SOURCE_DIR}/apriltag.c ${apriltaglib_SOURCE_DIR}/apriltag_pose.c ${apriltaglib_SOURCE_DIR}/apriltag_quad_thresh.c)

file(GLOB apriltag_jni_src src/main/native/cpp/jni/AprilTagJNI.cpp)

if (WITH_JAVA)
  find_package(Java REQUIRED)
  find_package(JNI REQUIRED)
  include(UseJava)
  set(CMAKE_JAVA_COMPILE_FLAGS "-encoding" "UTF8" "-Xlint:unchecked")

  set(CMAKE_JNI_TARGET true)

  file(GLOB EJML_JARS "${WPILIB_BINARY_DIR}/wpimath/thirdparty/ejml/*.jar")
  file(GLOB JACKSON_JARS "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson/*.jar")
  find_file(OPENCV_JAR_FILE
    NAMES opencv-${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.jar
    PATHS ${OPENCV_JAVA_INSTALL_DIR} ${OpenCV_INSTALL_PATH}/bin ${OpenCV_INSTALL_PATH}/share/java
    NO_DEFAULT_PATH)

  set(CMAKE_JAVA_INCLUDE_PATH apriltag.jar ${EJML_JARS} ${JACKSON_JARS})

  file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
  file(GLOB_RECURSE JAVA_RESOURCES src/main/native/resources/*.json)
  add_jar(apriltag_jar
    SOURCES ${JAVA_SOURCES}
    RESOURCES NAMESPACE "edu/wpi/first/apriltag" ${JAVA_RESOURCES}
    INCLUDE_JARS wpimath_jar ${EJML_JARS} wpiutil_jar ${OPENCV_JAR_FILE}
    OUTPUT_NAME apriltag
    GENERATE_NATIVE_HEADERS apriltag_jni_headers)

  get_property(APRILTAG_JAR_FILE TARGET apriltag_jar PROPERTY JAR_FILE)
  install(FILES ${APRILTAG_JAR_FILE} DESTINATION "${java_lib_dest}")

  set_property(TARGET apriltag_jar PROPERTY FOLDER "java")

  add_library(apriltagjni ${apriltag_jni_src})
  wpilib_target_warnings(apriltagjni)
  target_link_libraries(apriltagjni PUBLIC apriltag)

  set_property(TARGET apriltagjni PROPERTY FOLDER "libraries")

  target_link_libraries(apriltagjni PRIVATE apriltag_jni_headers)
  add_dependencies(apriltagjni apriltag_jar)

  if (MSVC)
    install(TARGETS apriltagjni RUNTIME DESTINATION "${jni_lib_dest}" COMPONENT Runtime)
  endif()

  install(TARGETS apriltagjni EXPORT apriltagjni DESTINATION "${main_lib_dest}")

endif()

GENERATE_RESOURCES(src/main/native/resources/edu/wpi/first/apriltag generated/main/cpp APRILTAG frc apriltag_resources_src)

file(GLOB apriltag_native_src src/main/native/cpp/*.cpp)

add_library(apriltag ${apriltag_native_src} ${apriltag_resources_src} ${APRILTAGLIB_SRCS} ${APRILTAGLIB_COMMON_SRC} ${TAG_FILES})
set_target_properties(apriltag PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET apriltag PROPERTY FOLDER "libraries")
target_compile_features(apriltag PUBLIC cxx_std_20)
wpilib_target_warnings(apriltag)
# disable warnings that apriltaglib can't handle
if (MSVC)
  target_compile_options(apriltag PRIVATE /wd4018)
else()
  target_compile_options(apriltag PRIVATE -Wno-sign-compare -Wno-gnu-zero-variadic-macro-arguments)
endif()

target_link_libraries(apriltag wpimath)

target_include_directories(apriltag PUBLIC
  $<BUILD_INTERFACE:${apriltaglib_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
  $<INSTALL_INTERFACE:${include_dest}/apriltag>)

install(TARGETS apriltag EXPORT apriltag DESTINATION "${main_lib_dest}")
install(DIRECTORY src/main/native/include/ DESTINATION "${include_dest}/apriltag")

if (WITH_JAVA AND MSVC)
    install(TARGETS apriltag RUNTIME DESTINATION "${jni_lib_dest}" COMPONENT Runtime)
endif()

if (WITH_FLAT_INSTALL)
    set (apriltag_config_dir ${wpilib_dest})
else()
    set (apriltag_config_dir share/apriltag)
endif()

configure_file(apriltag-config.cmake.in ${WPILIB_BINARY_DIR}/apriltag-config.cmake )
install(FILES ${WPILIB_BINARY_DIR}/apriltag-config.cmake DESTINATION ${apriltag_config_dir})
install(EXPORT apriltag DESTINATION ${apriltag_config_dir})

if (WITH_TESTS)
    wpilib_add_test(apriltag src/test/native/cpp)
    target_include_directories(apriltag_test PRIVATE src/test/native/include)
    target_link_libraries(apriltag_test apriltag gmock_main)
endif()

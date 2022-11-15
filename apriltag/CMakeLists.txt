project(fieldImages)

include(CompileWarnings)
include(GenResources)

if (WITH_JAVA)
     find_package(Java REQUIRED)
     include(UseJava)

     file(GLOB JACKSON_JARS "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson/*.jar")

     set(CMAKE_JAVA_INCLUDE_PATH apriltags.jar ${JACKSON_JARS})

     file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
     file(GLOB_RECURSE JAVA_RESOURCES src/main/native/resources/*.json)
     add_jar(apriltags_jar SOURCES ${JAVA_SOURCES} RESOURCES NAMESPACE "edu/wpi/first/apriltag" ${JAVA_RESOURCES} INCLUDE_JARS wpimath_jar OUTPUT_NAME apriltag)
endif()


GENERATE_RESOURCES(src/main/native/resources/edu/wpi/first/apriltag generated/main/cpp APRILTAG frc apriltags_resources_src)

file(GLOB_RECURSE apriltags_native_src src/main/native/cpp/*.cpp)

add_library(apriltags ${apriltags_native_src} ${apriltags_resources_src})
set_target_properties(apriltags PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET apriltags PROPERTY FOLDER "libraries")
target_compile_features(apriltags PUBLIC cxx_std_20)
wpilib_target_warnings(apriltags)
target_link_libraries(apriltags wpimath)

target_include_directories(apriltags PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
                            $<INSTALL_INTERFACE:${include_dest}/apriltags>)


if (WITH_TESTS)
    wpilib_add_test(apriltags src/test/native/cpp)
    target_include_directories(apriltags_test PRIVATE src/test/native/include)
    target_link_libraries(apriltags_test apriltags gmock_main)
endif()

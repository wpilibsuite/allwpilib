project(fieldImages)

include(CompileWarnings)
include(GenResources)

if (WITH_JAVA)
     find_package(Java REQUIRED)
     include(UseJava)

     file(GLOB JACKSON_JARS "${WPILIB_BINARY_DIR}/wpiutil/thirdparty/jackson/*.jar")

     set(CMAKE_JAVA_INCLUDE_PATH apriltag.jar ${JACKSON_JARS})

     file(GLOB_RECURSE JAVA_SOURCES src/main/java/*.java)
     file(GLOB_RECURSE JAVA_RESOURCES src/main/native/resources/*.json)
     add_jar(apriltag_jar SOURCES ${JAVA_SOURCES} RESOURCES NAMESPACE "edu/wpi/first/apriltag" ${JAVA_RESOURCES} INCLUDE_JARS wpimath_jar OUTPUT_NAME apriltag)
endif()


GENERATE_RESOURCES(src/main/native/resources/edu/wpi/first/apriltag generated/main/cpp APRILTAG frc apriltag_resources_src)

file(GLOB_RECURSE apriltag_native_src src/main/native/cpp/*.cpp)

add_library(apriltag ${apriltag_native_src} ${apriltag_resources_src})
set_target_properties(apriltag PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET apriltag PROPERTY FOLDER "libraries")
target_compile_features(apriltag PUBLIC cxx_std_20)
wpilib_target_warnings(apriltag)
target_link_libraries(apriltag wpimath)

target_include_directories(apriltag PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/main/native/include>
                            $<INSTALL_INTERFACE:${include_dest}/apriltag>)


if (WITH_TESTS)
    wpilib_add_test(apriltag src/test/native/cpp)
    target_include_directories(apriltag_test PRIVATE src/test/native/include)
    target_link_libraries(apriltag_test apriltag gmock_main)
endif()

project(developerRobot)

include(CompileWarnings)

file(GLOB developerRobotCpp_src src/main/native/cpp/*.cpp)

add_executable(developerRobotCpp ${developerRobotCpp_src})
target_link_libraries(developerRobotCpp wpilibc wpilibNewCommands apriltag)

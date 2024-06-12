project(myRobot)

include(CompileWarnings)

file(GLOB myRobotCpp_src src/main/native/cpp/*.cpp)

add_executable(myRobotCpp ${myRobotCpp_src})
target_link_libraries(myRobotCpp wpilibc)

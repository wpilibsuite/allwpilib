cmake_minimum_required(VERSION 2.8.3)
project(frc_gazebo_plugins)

set (PLUGINS
	clock
	dc_motor
	encoder
	gyro
	limit_switch
	pneumatic_piston
	potentiometer
	rangefinder
	servo)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/plugins)

link_directories(${GAZEBO_LIBRARY_DIRS})

foreach(PLUGIN ${PLUGINS})

	file(GLOB_RECURSE SRC_FILES ${PLUGIN}/src/*.cpp)

  include_directories(src ${Boost_INCLUDE_DIR} ${GZ_MSGS_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS} ${GAZEBO_INCLUDE_DIRS})

	if (WIN32)
		add_library(${PLUGIN} ${SRC_FILES})
	else()
		add_library(${PLUGIN} SHARED ${SRC_FILES})
	endif()

	target_link_libraries(${PLUGIN} gz_msgs ${GAZEBO_LIBRARIES} ${Boost_LIBRARIES})

  install(TARGETS ${PLUGIN} DESTINATION $ENV{HOME}/wpilib/simulation/plugins)

endforeach()

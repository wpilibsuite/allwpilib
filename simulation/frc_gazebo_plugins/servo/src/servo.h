/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "simulation/gz_msgs/msgs.h"

#include <gazebo/gazebo.hh>

using namespace gazebo;

/**
 * \brief Plugin for controlling a servo.
 *
 * This plugin subscribes to a topic to get a signal in the range
 * [-1,1]. Every physics update the joint's torque is set as
 * multiplier*signal.
 *
 * To add a servo to your robot, add the following XML to your robot
 * model:
 *
 *     <plugin name="my_servo" filename="libgz_servo.so">
 *       <joint>Joint Name</joint>
 *       <topic>/gzebo/frc/simulator/pwm/1</topic>
 *     </plugin>
 *
 * - `link`: Name of the link the servo is attached to.
 * - `topic`: Optional. Message type should be gazebo.msgs.Float64.
 */
class Servo: public ModelPlugin {
public:
	Servo();
	~Servo();

	/// \brief load the servo and configure it according to the sdf
	void Load(physics::ModelPtr model, sdf::ElementPtr sdf);

	/// \brief Update the torque on the joint from the dc motor each timestep.
	void Update(const common::UpdateInfo &info);

private:
	/// \brief Topic to read control signal from.
	std::string topic;

	/// \brief the pwm signal limited to the range [-1,1]
	double signal;

	/// \brief the torque of the motor in kg/cm
	double torque;

	/// \brief the joint that this servo moves
	physics::JointPtr joint;

	/// \brief Callback for receiving msgs and storing the signal
	void Callback(const msgs::ConstFloat64Ptr &msg);

	physics::ModelPtr model; 			///< \brief The model that this is attached to
	event::ConnectionPtr updateConn; 	///< \brief The Pointer to the world update function
	transport::NodePtr node; 			///< \brief The node we're advertising torque on
	transport::SubscriberPtr sub; 		///< \brief the Subscriber for the pwm signal

};

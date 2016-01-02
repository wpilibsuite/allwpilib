/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/


#ifndef _SIM_ENCODER_H
#define _SIM_ENCODER_H

#include "simulation/gz_msgs/msgs.h"
#include <gazebo/transport/transport.hh>
#include <gazebo/common/Time.hh>

using namespace gazebo;

class SimEncoder {
public:
	SimEncoder(std::string topic);

	void Reset();
	void Start();
	void Stop();
	double GetPosition();
	double GetVelocity();

private:
	void sendCommand(std::string cmd);

	double position, velocity;
	transport::SubscriberPtr posSub, velSub;
	transport::PublisherPtr commandPub;
	void positionCallback(const msgs::ConstFloat64Ptr &msg);
	void velocityCallback(const msgs::ConstFloat64Ptr &msg);
};

#endif

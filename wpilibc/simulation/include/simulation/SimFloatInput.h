/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/


#ifndef _SIM_FLOAT_INPUT_H
#define _SIM_FLOAT_INPUT_H

#include "simulation/gz_msgs/msgs.h"
#include <gazebo/transport/transport.hh>

using namespace gazebo;

class SimFloatInput {
public:
	SimFloatInput(std::string topic);

	/**
	 * @return The value of the potentiometer.
	 */
	double Get();

private:
	double value;
    transport::SubscriberPtr sub;
    void callback(const msgs::ConstFloat64Ptr &msg);
};

#endif

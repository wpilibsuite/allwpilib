/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/


#ifndef _SIM_DIGITAL_INPUT_H
#define _SIM_DIGITAL_INPUT_H

#include "simulation/gz_msgs/msgs.h"
#include <gazebo/transport/transport.hh>

using namespace gazebo;

class SimDigitalInput {
public:
	SimDigitalInput(std::string topic);

	/**
	 * @return The value of the potentiometer.
	 */
	bool Get();

private:
	bool value;
    transport::SubscriberPtr sub;
    void callback(const msgs::ConstBoolPtr &msg);
};

#endif

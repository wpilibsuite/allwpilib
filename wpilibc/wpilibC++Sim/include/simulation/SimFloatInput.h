

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

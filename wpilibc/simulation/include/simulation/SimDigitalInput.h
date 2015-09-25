

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



#ifndef _SIM_SPEED_CONTROLLER_H
#define _SIM_SPEED_CONTROLLER_H

#include <gazebo/transport/transport.hh>
#include "SpeedController.h"

using namespace gazebo;

class SimContinuousOutput {
private:
	transport::PublisherPtr pub;
	float speed;

public:
	SimContinuousOutput(std::string topic);

	/**
	 * Set the output value.
	 *
	 * The value is set using a range of -1.0 to 1.0, appropriately
	 * scaling the value.
	 *
	 * @param value The value between -1.0 and 1.0 to set.
	 */
	void Set(float value);

	/**
	 * @return The most recently set value.
	 */
	float Get();
};

#endif

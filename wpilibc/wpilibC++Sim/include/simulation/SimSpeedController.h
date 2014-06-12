

#ifndef _SIM_SPEED_CONTROLLER_H
#define _SIM_SPEED_CONTROLLER_H

#include <gazebo/transport/transport.hh>
#include "SpeedController.h"

using namespace gazebo;

class SimSpeedController : public SpeedController {
private:
	transport::PublisherPtr pub;
	float speed;

public:
	/**
	 * Constructor that assumes the default digital module.
	 *
	 * @param channel The PWM channel on the digital module that the Victor is attached to.
	 */
	SimSpeedController(std::string topic);

	/**
	 * Set the PWM value.
	 *
	 * @deprecated For compatibility with CANJaguar
	 *
	 * The PWM value is set using a range of -1.0 to 1.0, appropriately
	 * scaling the value for the FPGA.
	 *
	 * @param speed The speed to set.  Value should be between -1.0 and 1.0.
	 * @param syncGroup The update group to add this Set() to, pending UpdateSyncGroup().  If 0, update immediately.
	 */
	void Set(float speed, uint8_t syncGroup);

	/**
	 * Set the PWM value.
	 *
	 * The PWM value is set using a range of -1.0 to 1.0, appropriately
	 * scaling the value for the FPGA.
	 *
	 * @param speed The speed value between -1.0 and 1.0 to set.
	 */
	void Set(float speed);

	/**
	 * Get the recently set value of the PWM.
	 *
	 * @return The most recently set value for the PWM between -1.0 and 1.0.
	 */
	float Get();

	/**
	 * Disable the speed controller
	 */
	void Disable();

	/**
	 * Write out the PID value as seen in the PIDOutput base object.
	 *
	 * @param output Write out the PWM value as was found in the PIDController
	 */
	void PIDWrite(float output);
};

#endif

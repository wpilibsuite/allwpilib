#include "WPILib.h"

/**
 * This is a quick sample program to show how to use the new Talon SRX over CAN.
 * As of the time of this writing (11/20/14), the only mode supported on the SRX is the
 * straight-up throttle (status info, such as current and temperature should work).
 *
 */
class Robot : public SampleRobot {
	CANTalon m_motor;

	// update every 0.005 seconds/5 milliseconds.
	double kUpdatePeriod = 0.005;

public:
 Robot()
     : m_motor(1)   // Initialize the Talon as device 1. Use the roboRIO web
                    // interface to change the device number on the talons.
 {}

	/**
	 * Runs the motor from the output of a Joystick.
	 */
	void OperatorControl() {
    m_motor.EnableControl();
		while (IsOperatorControl() && IsEnabled()) {
      // Takes a number from -1.0 (full reverse) to +1.0 (full forwards).
			m_motor.Set(0.5);

			Wait(kUpdatePeriod); // Wait 5ms for the next update.
		}
    m_motor.Set(0.0);
	}
};

START_ROBOT_CLASS(Robot);

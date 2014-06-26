#ifndef Collector_H
#define Collector_H

#include "Commands/Subsystem.h"
#include "WPILib.h"

/**
 * The Collector subsystem has one motor for the rollers, a limit switch for ball
 * detection, a piston for opening and closing the claw, and a reed switch to
 * check if the piston is open.
 */
class Collector: public Subsystem
{
public:
	// Constants for some useful speeds
	static const double FORWARD = 1;
	static const double STOP = 0;
	static const double REVERSE = -1;

private:
	// Subsystem devices
	SpeedController* rollerMotor;
	DigitalInput* ballDetector;
	Solenoid* piston;
	DigitalInput* openDetector;

public:
	Collector();

	/**
	 * NOTE: The current simulation model uses the the lower part of the claw
	 * since the limit switch wasn't exported. At some point, this will be
	 * updated.
	 *
	 * @return Whether or not the robot has the ball.
	 */
	bool HasBall();

	/**
	 * @param speed The speed to spin the rollers.
	 */
	void SetSpeed(double speed);

	/**
	 * Stop the rollers from spinning
	 */
	void Stop();

	/**
	 * @return Whether or not the claw is open.
	 */
	bool IsOpen();

	/**
	 * Open the claw up. (For shooting)
	 */
	void Open();

	/**
	 * Close the claw. (For collecting and driving)
	 */
	void Close();

	/**
	 * No default command.
	 */
	void InitDefaultCommand();
};

#endif

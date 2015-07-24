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
	static constexpr double FORWARD = 1;
	static constexpr double STOP = 0;
	static constexpr double REVERSE = -1;

private:
	// Subsystem devices
  std::shared_ptr<SpeedController> rollerMotor;
  std::shared_ptr<DigitalInput> ballDetector;
  std::shared_ptr<Solenoid> piston;
  std::shared_ptr<DigitalInput> openDetector;

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

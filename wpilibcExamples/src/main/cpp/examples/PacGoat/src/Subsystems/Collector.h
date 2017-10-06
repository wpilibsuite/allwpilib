#ifndef Collector_H
#define Collector_H

#include <Commands/Subsystem.h>
#include <DigitalInput.h>
#include <Solenoid.h>
#include <Victor.h>

/**
 * The Collector subsystem has one motor for the rollers, a limit switch for ball
 * detection, a piston for opening and closing the claw, and a reed switch to
 * check if the piston is open.
 */
class Collector: public frc::Subsystem {
public:
	// Constants for some useful speeds
	static constexpr double kForward = 1;
	static constexpr double kStop = 0;
	static constexpr double kReverse = -1;

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
	void InitDefaultCommand() override;

private:
	// Subsystem devices
	frc::Victor rollerMotor { 6 };
	frc::DigitalInput ballDetector { 10 };
	frc::Solenoid piston { 1 };
	frc::DigitalInput openDetector { 6 };
};

#endif  // Collector_H

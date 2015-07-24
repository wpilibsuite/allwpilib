#ifndef Pivot_H
#define Pivot_H

#include "Commands/PIDSubsystem.h"
#include "WPILib.h"

/**
 * The Pivot subsystem contains the Van-door motor and the pot for PID control
 * of angle of the pivot and claw.
 */
class Pivot: public PIDSubsystem
{
public:
	// Constants for some useful angles
	static constexpr double COLLECT = 105;
	static constexpr double LOW_GOAL = 90;
	static constexpr double SHOOT = 45;
	static constexpr double SHOOT_NEAR = 30;

private:
	// Subsystem devices
  std::shared_ptr<DigitalInput> upperLimitSwitch;
  std::shared_ptr<DigitalInput> lowerLimitSwitch;
  std::shared_ptr<Potentiometer> pot;
  std::shared_ptr<SpeedController> motor;

public:
	Pivot();

	/**
	 *  No default command, if PID is enabled, the current setpoint will be maintained.
	 */
	void InitDefaultCommand() {}

	/**
	 * @return The angle read in by the potentiometer
	 */
	double ReturnPIDInput() const;

	/**
	 * Set the motor speed based off of the PID output
	 */
	void UsePIDOutput(double output);

	/**
	 * @return If the pivot is at its upper limit.
	 */
	bool IsAtUpperLimit();

	/**
	 * @return If the pivot is at its lower limit.
	 */
	bool IsAtLowerLimit();

	/**
	 * @return The current angle of the pivot.
	 */
	double GetAngle();
};

#endif

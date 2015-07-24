#ifndef Wrist_H
#define Wrist_H

#include "Commands/PIDSubsystem.h"
#include "WPILib.h"

/**
 * The wrist subsystem is like the elevator, but with a rotational joint instead
 * of a linear joint.
 */
class Wrist : public PIDSubsystem {
private:
	SpeedController* motor;
    Potentiometer* pot; // TODO: Make Potentiometer

    static constexpr double kP_real = 1, kP_simulation = 0.05;

public:
    Wrist();
    void InitDefaultCommand() {}

	/**
	 * The log method puts interesting information to the SmartDashboard.
	 */
    void Log();

    /**
     * Use the potentiometer as the PID sensor. This method is automatically
     * called by the subsystem.
     */
    double ReturnPIDInput();

    /**
     * Use the motor as the PID output. This method is automatically called by
     * the subsystem.
     */
    void UsePIDOutput(double d);
};

#endif

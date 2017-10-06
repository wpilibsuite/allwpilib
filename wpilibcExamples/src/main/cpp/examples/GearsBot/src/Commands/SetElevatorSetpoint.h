#ifndef SetElevatorSetpoint_H
#define SetElevatorSetpoint_H

#include <Commands/Command.h>

/**
 * Move the elevator to a given location. This command finishes when it is within
 * the tolerance, but leaves the PID loop running to maintain the position. Other
 * commands using the elevator should make sure they disable PID!
 */
class SetElevatorSetpoint: public frc::Command {
public:
	SetElevatorSetpoint(double setpoint);
	void Initialize() override;
	bool IsFinished() override;

private:
	double setpoint;
};

#endif  // ElevatorSetpoint_H

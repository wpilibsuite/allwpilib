#ifndef SetPivotSetpoint_H
#define SetPivotSetpoint_H

#include <Commands/Command.h>

/**
 * Moves the  pivot to a given angle. This command finishes when it is within
 * the tolerance, but leaves the PID loop running to maintain the position.
 * Other commands using the pivot should make sure they disable PID!
 */
class SetPivotSetpoint: public frc::Command {
public:
	SetPivotSetpoint(double setpoint);
	void Initialize() override;
	bool IsFinished() override;

private:
	double setpoint;
};

#endif  // SetPivotSetpoint_H

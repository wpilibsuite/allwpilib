#include "SetPivotSetpoint.h"

#include "../Robot.h"

SetPivotSetpoint::SetPivotSetpoint(double setpoint) {
	this->setpoint = setpoint;
	Requires(Robot::pivot.get());
}

// Called just before this Command runs the first time
void SetPivotSetpoint::Initialize() {
	Robot::pivot->Enable();
	Robot::pivot->SetSetpoint(setpoint);
}

// Make this return true when this Command no longer needs to run execute()
bool SetPivotSetpoint::IsFinished() {
	return Robot::pivot->OnTarget();
}

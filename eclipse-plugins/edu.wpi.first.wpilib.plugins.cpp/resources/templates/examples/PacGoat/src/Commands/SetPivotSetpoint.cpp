#include "SetPivotSetpoint.h"

#include "Robot.h"

SetPivotSetpoint::SetPivotSetpoint(double setpoint) {
	this->setpoint = setpoint;
	Requires(Robot::pivot.get());
}

// Called just before this Command runs the first time
void SetPivotSetpoint::Initialize() {
	Robot::pivot->Enable();
	Robot::pivot->SetSetpoint(setpoint);
}

// Called repeatedly when this Command is scheduled to run
void SetPivotSetpoint::Execute() {}

// Make this return true when this Command no longer needs to run execute()
bool SetPivotSetpoint::IsFinished()
{
	return Robot::pivot->OnTarget();
}

// Called once after isFinished returns true
void SetPivotSetpoint::End() {}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void SetPivotSetpoint::Interrupted() {}

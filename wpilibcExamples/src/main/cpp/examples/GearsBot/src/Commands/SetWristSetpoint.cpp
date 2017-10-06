#include "SetWristSetpoint.h"

#include "../Robot.h"

SetWristSetpoint::SetWristSetpoint(double setpoint) :
		frc::Command("SetWristSetpoint") {
	this->setpoint = setpoint;
	Requires(Robot::wrist.get());
}

// Called just before this Command runs the first time
void SetWristSetpoint::Initialize() {
	Robot::wrist->SetSetpoint(setpoint);
	Robot::wrist->Enable();
}

// Make this return true when this Command no longer needs to run execute()
bool SetWristSetpoint::IsFinished() {
	return Robot::wrist->OnTarget();
}

#include "SetElevatorSetpoint.h"
#include "Robot.h"
#include <math.h>

SetElevatorSetpoint::SetElevatorSetpoint(double setpoint) : Command("SetElevatorSetpoint") {
	this->setpoint = setpoint;
	Requires(Robot::elevator.get());
}

// Called just before this Command runs the first time
void SetElevatorSetpoint::Initialize() {
	Robot::elevator->SetSetpoint(setpoint);
	Robot::elevator->Enable();
}

// Called repeatedly when this Command is scheduled to run
void SetElevatorSetpoint::Execute() {}

// Make this return true when this Command no longer needs to run execute()
bool SetElevatorSetpoint::IsFinished() {
	return Robot::elevator->OnTarget();
}

// Called once after isFinished returns true
void SetElevatorSetpoint::End() {}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void SetElevatorSetpoint::Interrupted() {}

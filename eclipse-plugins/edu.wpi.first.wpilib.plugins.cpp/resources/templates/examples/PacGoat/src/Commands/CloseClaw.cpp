#include "CloseClaw.h"

#include "Robot.h"

CloseClaw::CloseClaw() {
	Requires(Robot::collector.get());
}

// Called just before this Command runs the first time
void CloseClaw::Initialize() {
	Robot::collector->Close();
}

// Called repeatedly when this Command is scheduled to run
void CloseClaw::Execute() {}

// Make this return true when this Command no longer needs to run execute()
bool CloseClaw::IsFinished() {
	return true;
}

// Called once after isFinished returns true
void CloseClaw::End() {}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void CloseClaw::Interrupted() {}

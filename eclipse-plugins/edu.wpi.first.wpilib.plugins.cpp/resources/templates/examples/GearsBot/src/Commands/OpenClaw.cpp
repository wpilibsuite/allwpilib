#include "OpenClaw.h"
#include "Robot.h"

OpenClaw::OpenClaw() : Command("OpenClaw") {
	Requires(Robot::claw.get());
	SetTimeout(1);
}

// Called just before this Command runs the first time
void OpenClaw::Initialize() {
	Robot::claw->Open();
}

// Called repeatedly when this Command is scheduled to run
void OpenClaw::Execute() {}

// Make this return true when this Command no longer needs to run execute()
bool OpenClaw::IsFinished() {
	return IsTimedOut();
}

// Called once after isFinished returns true
void OpenClaw::End() {
	Robot::claw->Stop();
}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void OpenClaw::Interrupted() {
	End();
}

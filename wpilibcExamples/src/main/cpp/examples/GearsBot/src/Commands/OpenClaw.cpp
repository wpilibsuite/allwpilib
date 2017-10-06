#include "OpenClaw.h"

#include "../Robot.h"

OpenClaw::OpenClaw() :
		frc::Command("OpenClaw") {
	Requires(Robot::claw.get());
	SetTimeout(1);
}

// Called just before this Command runs the first time
void OpenClaw::Initialize() {
	Robot::claw->Open();
}

// Make this return true when this Command no longer needs to run execute()
bool OpenClaw::IsFinished() {
	return IsTimedOut();
}

// Called once after isFinished returns true
void OpenClaw::End() {
	Robot::claw->Stop();
}

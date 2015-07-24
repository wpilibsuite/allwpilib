#include "ExtendShooter.h"

#include "Robot.h"

ExtendShooter::ExtendShooter() {
	Requires(Robot::shooter.get());
	SetTimeout(1);
}

// Called just before this Command runs the first time
void ExtendShooter::Initialize() {
	Robot::shooter->ExtendBoth();
}

// Called repeatedly when this Command is scheduled to run
void ExtendShooter::Execute() {}

// Make this return true when this Command no longer needs to run execute()
bool ExtendShooter::IsFinished() {
	return IsTimedOut();
}

// Called once after isFinished returns true
void ExtendShooter::End() {
	Robot::shooter->RetractBoth();
}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void ExtendShooter::Interrupted() {
	End();
}

#include "WaitForBall.h"

#include "Robot.h"

WaitForBall::WaitForBall() {
	Requires(Robot::collector.get());
}

// Called just before this Command runs the first time
void WaitForBall::Initialize() {}

// Called repeatedly when this Command is scheduled to run
void WaitForBall::Execute() {}

// Make this return true when this Command no longer needs to run execute()
bool WaitForBall::IsFinished()
{
	return Robot::collector->HasBall();
}

// Called once after isFinished returns true
void WaitForBall::End() {}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void WaitForBall::Interrupted() {}

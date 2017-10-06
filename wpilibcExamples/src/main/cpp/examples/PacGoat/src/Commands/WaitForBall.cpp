#include "WaitForBall.h"

#include "../Robot.h"

WaitForBall::WaitForBall() {
	Requires(Robot::collector.get());
}

// Make this return true when this Command no longer needs to run execute()
bool WaitForBall::IsFinished() {
	return Robot::collector->HasBall();
}

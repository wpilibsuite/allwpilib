#include "CheckForHotGoal.h"

#include "Robot.h"

CheckForHotGoal::CheckForHotGoal(double time) {
	SetTimeout(time);
}

// Called just before this Command runs the first time
void CheckForHotGoal::Initialize() {}

// Called repeatedly when this Command is scheduled to run
void CheckForHotGoal::Execute() {}

// Make this return true when this Command no longer needs to run execute()
bool CheckForHotGoal::IsFinished() {
	return IsTimedOut() || Robot::shooter->GoalIsHot();
}

// Called once after isFinished returns true
void CheckForHotGoal::End() {}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void CheckForHotGoal::Interrupted() {}

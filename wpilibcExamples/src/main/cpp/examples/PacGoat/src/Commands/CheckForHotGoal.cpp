#include "CheckForHotGoal.h"

#include "../Robot.h"

CheckForHotGoal::CheckForHotGoal(double time) {
	SetTimeout(time);
}

// Make this return true when this Command no longer needs to run execute()
bool CheckForHotGoal::IsFinished() {
	return IsTimedOut() || Robot::shooter->GoalIsHot();
}

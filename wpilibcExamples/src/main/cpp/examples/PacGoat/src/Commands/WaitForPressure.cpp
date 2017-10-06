#include "WaitForPressure.h"

#include "../Robot.h"

WaitForPressure::WaitForPressure() {
	Requires(Robot::pneumatics.get());
}

// Make this return true when this Command no longer needs to run execute()
bool WaitForPressure::IsFinished() {
	return Robot::pneumatics->IsPressurized();
}

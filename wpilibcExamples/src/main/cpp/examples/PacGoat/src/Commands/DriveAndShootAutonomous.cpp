#include "DriveAndShootAutonomous.h"

#include "CheckForHotGoal.h"
#include "CloseClaw.h"
#include "DriveForward.h"
#include "SetPivotSetpoint.h"
#include "Shoot.h"
#include "WaitForPressure.h"
#include "../Robot.h"

DriveAndShootAutonomous::DriveAndShootAutonomous() {
	AddSequential(new CloseClaw());
	AddSequential(new WaitForPressure(), 2);
#ifndef SIMULATION
	// NOTE: Simulation doesn't currently have the concept of hot.
	AddSequential(new CheckForHotGoal(2));
#endif
	AddSequential(new SetPivotSetpoint(45));
	AddSequential(new DriveForward(8, 0.3));
	AddSequential(new Shoot());
}

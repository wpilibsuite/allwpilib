#include "DriveAndShootAutonomous.h"

#include "Robot.h"
#include "Commands/WaitForPressure.h"
#include "Commands/CloseClaw.h"
#include "Commands/SetPivotSetpoint.h"
#include "Commands/DriveForward.h"
#include "Commands/Shoot.h"
#include "Commands/CheckForHotGoal.h"

DriveAndShootAutonomous::DriveAndShootAutonomous() {
    AddSequential(new CloseClaw());
    AddSequential(new WaitForPressure(), 2);
 	#ifdef REAL
    	// NOTE: Simulation doesn't currently have the concept of hot.
    	AddSequential(new CheckForHotGoal(2));
	#endif
    AddSequential(new SetPivotSetpoint(45));
    AddSequential(new DriveForward(8, 0.3));
    AddSequential(new Shoot());
}

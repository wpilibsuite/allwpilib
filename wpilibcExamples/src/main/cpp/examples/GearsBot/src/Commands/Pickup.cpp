#include "Pickup.h"

#include "CloseClaw.h"
#include "SetElevatorSetpoint.h"
#include "SetWristSetpoint.h"

Pickup::Pickup() :
		frc::CommandGroup("Pickup") {
	AddSequential(new CloseClaw());
	AddParallel(new SetWristSetpoint(-45));
	AddSequential(new SetElevatorSetpoint(0.25));
}

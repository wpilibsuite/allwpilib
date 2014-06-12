#include "Pickup.h"
#include "CloseClaw.h"
#include "SetWristSetpoint.h"
#include "SetElevatorSetpoint.h"

#include <iostream>

Pickup::Pickup() : CommandGroup("Pickup") {
	AddSequential(new CloseClaw());
	AddParallel(new SetWristSetpoint(-45));
	AddSequential(new SetElevatorSetpoint(0.25));
}

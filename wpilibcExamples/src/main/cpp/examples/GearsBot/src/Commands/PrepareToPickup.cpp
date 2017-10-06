#include "PrepareToPickup.h"

#include "OpenClaw.h"
#include "SetElevatorSetpoint.h"
#include "SetWristSetpoint.h"

PrepareToPickup::PrepareToPickup() :
		frc::CommandGroup("PrepareToPickup") {
	AddParallel(new OpenClaw());
	AddParallel(new SetWristSetpoint(0));
	AddSequential(new SetElevatorSetpoint(0));
}

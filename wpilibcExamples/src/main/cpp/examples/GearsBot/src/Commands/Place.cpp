#include "Place.h"

#include "OpenClaw.h"
#include "SetWristSetpoint.h"
#include "SetElevatorSetpoint.h"

Place::Place() :
		frc::CommandGroup("Place") {
	AddSequential(new SetElevatorSetpoint(0.25));
	AddSequential(new SetWristSetpoint(0));
	AddSequential(new OpenClaw());
}

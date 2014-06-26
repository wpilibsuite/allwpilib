#include "Shoot.h"

#include "Robot.h"
#include "Commands/WaitForPressure.h"
#include "Commands/SetCollectionSpeed.h"
#include "Commands/OpenClaw.h"
#include "Commands/ExtendShooter.h"

Shoot::Shoot() {
	AddSequential(new WaitForPressure());
	AddSequential(new SetCollectionSpeed(Collector::STOP));
	AddSequential(new OpenClaw());
	AddSequential(new ExtendShooter());
}

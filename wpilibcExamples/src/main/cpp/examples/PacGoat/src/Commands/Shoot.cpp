#include "Shoot.h"

#include "ExtendShooter.h"
#include "OpenClaw.h"
#include "SetCollectionSpeed.h"
#include "WaitForPressure.h"
#include "../Robot.h"

Shoot::Shoot() {
	AddSequential(new WaitForPressure());
	AddSequential(new SetCollectionSpeed(Collector::kStop));
	AddSequential(new OpenClaw());
	AddSequential(new ExtendShooter());
}

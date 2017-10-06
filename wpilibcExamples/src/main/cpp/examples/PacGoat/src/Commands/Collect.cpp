#include "Collect.h"

#include "CloseClaw.h"
#include "SetCollectionSpeed.h"
#include "SetPivotSetpoint.h"
#include "WaitForBall.h"
#include "../Robot.h"

Collect::Collect() {
	AddSequential(new SetCollectionSpeed(Collector::kForward));
	AddParallel(new CloseClaw());
	AddSequential(new SetPivotSetpoint(Pivot::kCollect));
	AddSequential(new WaitForBall());
}

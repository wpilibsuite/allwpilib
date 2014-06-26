#include "Collect.h"

#include "Robot.h"
#include "Commands/SetCollectionSpeed.h"
#include "Commands/CloseClaw.h"
#include "Commands/SetPivotSetpoint.h"
#include "Commands/WaitForBall.h"

Collect::Collect() {
	AddSequential(new SetCollectionSpeed(Collector::FORWARD));
	AddParallel(new CloseClaw());
	AddSequential(new SetPivotSetpoint(Pivot::COLLECT));
	AddSequential(new WaitForBall());
}

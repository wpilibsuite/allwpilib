#include "LowGoal.h"

#include "ExtendShooter.h"
#include "SetCollectionSpeed.h"
#include "SetPivotSetpoint.h"
#include "../Robot.h"

LowGoal::LowGoal() {
	AddSequential(new SetPivotSetpoint(Pivot::kLowGoal));
	AddSequential(new SetCollectionSpeed(Collector::kReverse));
	AddSequential(new ExtendShooter());
}

/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/LowGoal.h"

#include "Commands/ExtendShooter.h"
#include "Commands/SetCollectionSpeed.h"
#include "Commands/SetPivotSetpoint.h"
#include "Robot.h"

LowGoal::LowGoal() {
  AddSequential(new SetPivotSetpoint(Pivot::kLowGoal));
  AddSequential(new SetCollectionSpeed(Collector::kReverse));
  AddSequential(new ExtendShooter());
}

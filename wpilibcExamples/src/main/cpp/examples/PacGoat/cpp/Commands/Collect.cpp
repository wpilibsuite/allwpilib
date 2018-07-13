/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/Collect.h"

#include "Commands/CloseClaw.h"
#include "Commands/SetCollectionSpeed.h"
#include "Commands/SetPivotSetpoint.h"
#include "Commands/WaitForBall.h"
#include "Robot.h"

Collect::Collect() {
  AddSequential(new SetCollectionSpeed(Collector::kForward));
  AddParallel(new CloseClaw());
  AddSequential(new SetPivotSetpoint(Pivot::kCollect));
  AddSequential(new WaitForBall());
}

/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/Shoot.h"

#include "Commands/ExtendShooter.h"
#include "Commands/OpenClaw.h"
#include "Commands/SetCollectionSpeed.h"
#include "Commands/WaitForPressure.h"
#include "Robot.h"

Shoot::Shoot() {
  AddSequential(new WaitForPressure());
  AddSequential(new SetCollectionSpeed(Collector::kStop));
  AddSequential(new OpenClaw());
  AddSequential(new ExtendShooter());
}

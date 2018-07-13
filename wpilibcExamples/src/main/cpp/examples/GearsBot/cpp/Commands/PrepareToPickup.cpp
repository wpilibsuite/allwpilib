/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/PrepareToPickup.h"

#include "Commands/OpenClaw.h"
#include "Commands/SetElevatorSetpoint.h"
#include "Commands/SetWristSetpoint.h"

PrepareToPickup::PrepareToPickup() : frc::CommandGroup("PrepareToPickup") {
  AddParallel(new OpenClaw());
  AddParallel(new SetWristSetpoint(0));
  AddSequential(new SetElevatorSetpoint(0));
}

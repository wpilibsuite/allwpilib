/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/Pickup.h"

#include "Commands/CloseClaw.h"
#include "Commands/SetElevatorSetpoint.h"
#include "Commands/SetWristSetpoint.h"

Pickup::Pickup() : frc::CommandGroup("Pickup") {
  AddSequential(new CloseClaw());
  AddParallel(new SetWristSetpoint(-45));
  AddSequential(new SetElevatorSetpoint(0.25));
}

/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/Place.h"

#include "Commands/OpenClaw.h"
#include "Commands/SetElevatorSetpoint.h"
#include "Commands/SetWristSetpoint.h"

Place::Place() : frc::CommandGroup("Place") {
  AddSequential(new SetElevatorSetpoint(0.25));
  AddSequential(new SetWristSetpoint(0));
  AddSequential(new OpenClaw());
}

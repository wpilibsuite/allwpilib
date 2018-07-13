/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/Autonomous.h"

#include "Commands/CloseClaw.h"
#include "Commands/DriveStraight.h"
#include "Commands/Pickup.h"
#include "Commands/Place.h"
#include "Commands/PrepareToPickup.h"
#include "Commands/SetDistanceToBox.h"
#include "Commands/SetWristSetpoint.h"

Autonomous::Autonomous() : frc::CommandGroup("Autonomous") {
  AddSequential(new PrepareToPickup());
  AddSequential(new Pickup());
  AddSequential(new SetDistanceToBox(0.10));
  // AddSequential(new DriveStraight(4));  // Use Encoders if ultrasonic
  // is broken
  AddSequential(new Place());
  AddSequential(new SetDistanceToBox(0.60));
  // AddSequential(new DriveStraight(-2));  // Use Encoders if ultrasonic
  // is broken
  AddParallel(new SetWristSetpoint(-45));
  AddSequential(new CloseClaw());
}

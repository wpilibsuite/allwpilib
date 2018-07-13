/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/ExtendShooter.h"

#include "Robot.h"

ExtendShooter::ExtendShooter() : frc::TimedCommand(1.0) {
  Requires(&Robot::shooter);
}

// Called just before this Command runs the first time
void ExtendShooter::Initialize() { Robot::shooter.ExtendBoth(); }

// Called once after isFinished returns true
void ExtendShooter::End() { Robot::shooter.RetractBoth(); }

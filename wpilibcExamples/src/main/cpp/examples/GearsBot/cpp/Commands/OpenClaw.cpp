/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/OpenClaw.h"

#include "Robot.h"

OpenClaw::OpenClaw() : frc::Command("OpenClaw") {
  Requires(&Robot::claw);
  SetTimeout(1);
}

// Called just before this Command runs the first time
void OpenClaw::Initialize() { Robot::claw.Open(); }

// Make this return true when this Command no longer needs to run execute()
bool OpenClaw::IsFinished() { return IsTimedOut(); }

// Called once after isFinished returns true
void OpenClaw::End() { Robot::claw.Stop(); }

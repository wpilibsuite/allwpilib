/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/WaitForBall.h"

#include "Robot.h"

WaitForBall::WaitForBall() { Requires(&Robot::collector); }

// Make this return true when this Command no longer needs to run execute()
bool WaitForBall::IsFinished() { return Robot::collector.HasBall(); }

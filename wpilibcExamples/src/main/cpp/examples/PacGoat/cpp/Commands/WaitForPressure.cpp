/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/WaitForPressure.h"

#include "Robot.h"

WaitForPressure::WaitForPressure() { Requires(&Robot::pneumatics); }

// Make this return true when this Command no longer needs to run execute()
bool WaitForPressure::IsFinished() { return Robot::pneumatics.IsPressurized(); }

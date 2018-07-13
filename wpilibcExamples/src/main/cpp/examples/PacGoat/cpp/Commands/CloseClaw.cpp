/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/CloseClaw.h"

#include "Robot.h"

CloseClaw::CloseClaw() { Requires(&Robot::collector); }

// Called just before this Command runs the first time
void CloseClaw::Initialize() { Robot::collector.Close(); }

/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/SetPivotSetpoint.h"

#include "Robot.h"

SetPivotSetpoint::SetPivotSetpoint(double setpoint) {
  m_setpoint = setpoint;
  Requires(&Robot::pivot);
}

// Called just before this Command runs the first time
void SetPivotSetpoint::Initialize() {
  Robot::pivot.Enable();
  Robot::pivot.SetSetpoint(m_setpoint);
}

// Make this return true when this Command no longer needs to run execute()
bool SetPivotSetpoint::IsFinished() { return Robot::pivot.OnTarget(); }

/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/SetWristSetpoint.h"

#include "Robot.h"

SetWristSetpoint::SetWristSetpoint(double setpoint)
    : frc::Command("SetWristSetpoint") {
  m_setpoint = setpoint;
  Requires(&Robot::wrist);
}

// Called just before this Command runs the first time
void SetWristSetpoint::Initialize() {
  Robot::wrist.SetSetpoint(m_setpoint);
  Robot::wrist.Enable();
}

// Make this return true when this Command no longer needs to run execute()
bool SetWristSetpoint::IsFinished() { return Robot::wrist.OnTarget(); }

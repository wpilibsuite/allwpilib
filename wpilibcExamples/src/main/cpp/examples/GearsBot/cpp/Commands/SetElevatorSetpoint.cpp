/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/SetElevatorSetpoint.h"

#include <cmath>

#include "Robot.h"

SetElevatorSetpoint::SetElevatorSetpoint(double setpoint)
    : frc::Command("SetElevatorSetpoint") {
  m_setpoint = setpoint;
  Requires(&Robot::elevator);
}

// Called just before this Command runs the first time
void SetElevatorSetpoint::Initialize() {
  Robot::elevator.SetSetpoint(m_setpoint);
  Robot::elevator.Enable();
}

// Make this return true when this Command no longer needs to run execute()
bool SetElevatorSetpoint::IsFinished() { return Robot::elevator.OnTarget(); }

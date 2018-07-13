/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/SetDistanceToBox.h"

#include <PIDController.h>

#include "Robot.h"

SetDistanceToBox::SetDistanceToBox(double distance) {
  Requires(&Robot::drivetrain);
  m_pid.SetAbsoluteTolerance(0.01);
  m_pid.SetSetpoint(distance);
}

// Called just before this Command runs the first time
void SetDistanceToBox::Initialize() {
  // Get everything in a safe starting state.
  Robot::drivetrain.Reset();
  m_pid.Reset();
  m_pid.Enable();
}

// Make this return true when this Command no longer needs to run execute()
bool SetDistanceToBox::IsFinished() { return m_pid.OnTarget(); }

// Called once after isFinished returns true
void SetDistanceToBox::End() {
  // Stop PID and the wheels
  m_pid.Disable();
  Robot::drivetrain.Drive(0, 0);
}

double SetDistanceToBox::SetDistanceToBoxPIDSource::PIDGet() {
  return Robot::drivetrain.GetDistanceToObstacle();
}

void SetDistanceToBox::SetDistanceToBoxPIDOutput::PIDWrite(double d) {
  Robot::drivetrain.Drive(d, d);
}

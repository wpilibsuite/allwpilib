/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/DriveStraight.h"

#include "Robot.h"

DriveStraight::DriveStraight(double distance) {
  Requires(&Robot::drivetrain);
  m_pid.SetAbsoluteTolerance(0.01);
  m_pid.SetSetpoint(distance);
}

// Called just before this Command runs the first time
void DriveStraight::Initialize() {
  // Get everything in a safe starting state.
  Robot::drivetrain.Reset();
  m_pid.Reset();
  m_pid.Enable();
}

// Make this return true when this Command no longer needs to run execute()
bool DriveStraight::IsFinished() { return m_pid.OnTarget(); }

// Called once after isFinished returns true
void DriveStraight::End() {
  // Stop PID and the wheels
  m_pid.Disable();
  Robot::drivetrain.Drive(0, 0);
}

double DriveStraight::DriveStraightPIDSource::PIDGet() {
  return Robot::drivetrain.GetDistance();
}

void DriveStraight::DriveStraightPIDOutput::PIDWrite(double d) {
  Robot::drivetrain.Drive(d, d);
}

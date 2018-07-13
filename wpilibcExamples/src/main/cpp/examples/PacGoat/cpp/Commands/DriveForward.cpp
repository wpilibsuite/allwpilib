/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Commands/DriveForward.h"

#include <cmath>

#include "Robot.h"

void DriveForward::init(double dist, double maxSpeed) {
  Requires(&Robot::drivetrain);
  m_distance = dist;
  m_driveForwardSpeed = maxSpeed;
}

DriveForward::DriveForward() { init(10, 0.5); }

DriveForward::DriveForward(double dist) { init(dist, 0.5); }

DriveForward::DriveForward(double dist, double maxSpeed) {
  init(dist, maxSpeed);
}

// Called just before this Command runs the first time
void DriveForward::Initialize() {
  Robot::drivetrain.GetRightEncoder().Reset();
  SetTimeout(2);
}

// Called repeatedly when this Command is scheduled to run
void DriveForward::Execute() {
  m_error = (m_distance - Robot::drivetrain.GetRightEncoder().GetDistance());
  if (m_driveForwardSpeed * kP * m_error >= m_driveForwardSpeed) {
    Robot::drivetrain.TankDrive(m_driveForwardSpeed, m_driveForwardSpeed);
  } else {
    Robot::drivetrain.TankDrive(m_driveForwardSpeed * kP * m_error,
                                m_driveForwardSpeed * kP * m_error);
  }
}

// Make this return true when this Command no longer needs to run execute()
bool DriveForward::IsFinished() {
  return (std::fabs(m_error) <= kTolerance) || IsTimedOut();
}

// Called once after isFinished returns true
void DriveForward::End() { Robot::drivetrain.Stop(); }

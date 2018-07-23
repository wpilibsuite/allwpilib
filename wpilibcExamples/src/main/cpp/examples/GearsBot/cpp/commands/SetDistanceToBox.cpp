/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/SetDistanceToBox.h"

#include <frc/PIDController.h>

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

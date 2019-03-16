/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "commands/SetDistanceToBox.h"

SetDistanceToBox::SetDistanceToBox(double distance) {
  Requires(&Robot::drivetrain);
  m_pidController.SetAbsoluteTolerance(0.01);
  m_pidController.SetReference(distance);
}

// Called just before this Command runs the first time
void SetDistanceToBox::Initialize() {
  // Get everything in a safe starting state.
  Robot::drivetrain.Reset();
  m_pidController.Reset();
  m_pidRunner.Enable();
}

// Make this return true when this Command no longer needs to run execute()
bool SetDistanceToBox::IsFinished() { return m_pidController.AtReference(); }

// Called once after isFinished returns true
void SetDistanceToBox::End() {
  // Stop PID and the wheels
  m_pidRunner.Disable();
  Robot::drivetrain.Drive(0, 0);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/DriveWithJoystick.h"

#include "Robot.h"

DriveWithJoystick::DriveWithJoystick() {
  Requires(&Robot::drivetrain);
}

// Called repeatedly when this Command is scheduled to run
void DriveWithJoystick::Execute() {
  auto& joystick = Robot::oi.GetJoystick();
  Robot::drivetrain.TankDrive(-joystick.GetY(), joystick.GetRawAxis(4));
}

// Make this return true when this Command no longer needs to run execute()
bool DriveWithJoystick::IsFinished() {
  return false;
}

// Called once after isFinished returns true
void DriveWithJoystick::End() {
  Robot::drivetrain.Stop();
}

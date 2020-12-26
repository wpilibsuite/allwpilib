// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/RobotState.h"

#include "frc/DriverStation.h"

using namespace frc;

bool RobotState::IsDisabled() {
  return DriverStation::GetInstance().IsDisabled();
}

bool RobotState::IsEnabled() {
  return DriverStation::GetInstance().IsEnabled();
}

bool RobotState::IsEStopped() {
  return DriverStation::GetInstance().IsEStopped();
}

bool RobotState::IsOperatorControl() {
  return DriverStation::GetInstance().IsOperatorControl();
}

bool RobotState::IsAutonomous() {
  return DriverStation::GetInstance().IsAutonomous();
}

bool RobotState::IsTest() { return DriverStation::GetInstance().IsTest(); }

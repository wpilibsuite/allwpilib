// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/opmode/RobotState.hpp"

#include "wpi/driverstation/DriverStation.hpp"

using namespace frc;

bool RobotState::IsDisabled() {
  return DriverStation::IsDisabled();
}

bool RobotState::IsEnabled() {
  return DriverStation::IsEnabled();
}

bool RobotState::IsEStopped() {
  return DriverStation::IsEStopped();
}

bool RobotState::IsTeleop() {
  return DriverStation::IsTeleop();
}

bool RobotState::IsAutonomous() {
  return DriverStation::IsAutonomous();
}

bool RobotState::IsTest() {
  return DriverStation::IsTest();
}

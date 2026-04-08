// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/button/RobotModeTriggers.hpp"

#include "wpi/driverstation/DriverStation.hpp"

using namespace wpi::cmd;

Trigger RobotModeTriggers::Autonomous() {
  return Trigger{&wpi::DriverStation::IsAutonomousEnabled};
}

Trigger RobotModeTriggers::Teleop() {
  return Trigger{&wpi::DriverStation::IsTeleopEnabled};
}

Trigger RobotModeTriggers::Disabled() {
  return Trigger{&wpi::DriverStation::IsDisabled};
}

Trigger RobotModeTriggers::Test() {
  return Trigger{&wpi::DriverStation::IsTestEnabled};
}

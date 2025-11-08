// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/button/RobotModeTriggers.hpp"

#include "wpi/driverstation/DriverStation.hpp"

using namespace frc2;

Trigger RobotModeTriggers::Autonomous() {
  return Trigger{&frc::DriverStation::IsAutonomousEnabled};
}

Trigger RobotModeTriggers::Teleop() {
  return Trigger{&frc::DriverStation::IsTeleopEnabled};
}

Trigger RobotModeTriggers::Disabled() {
  return Trigger{&frc::DriverStation::IsDisabled};
}

Trigger RobotModeTriggers::Test() {
  return Trigger{&frc::DriverStation::IsTestEnabled};
}

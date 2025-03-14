// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/RobotModeTriggers.h"

#include <frc/DriverStation.h>

using namespace frc2;

Trigger RobotModeTriggers::OpMode(int mode) {
  return Trigger{[=] { return frc::DriverStation::IsOpMode(mode); }};
}

Trigger RobotModeTriggers::OpMode(std::string_view mode) {
  return Trigger{[=] { return frc::DriverStation::IsOpMode(mode); }};
}

Trigger RobotModeTriggers::Disabled() {
  return Trigger{&frc::DriverStation::IsDisabled};
}

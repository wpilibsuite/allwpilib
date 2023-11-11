// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc2/command/button/GameTriggers.h"

#include <frc/DriverStation.h>

using namespace frc2;

Trigger GameTriggers::Autonomous() {
  return Trigger{&frc::DriverStation::IsAutonomousEnabled};
}

Trigger GameTriggers::Teleop() {
  return Trigger{&frc::DriverStation::IsTeleopEnabled};
}

Trigger GameTriggers::Disabled() {
  return Trigger{&frc::DriverStation::IsDisabled};
}

Trigger GameTriggers::Test() {
  return Trigger{&frc::DriverStation::IsTestEnabled};
}

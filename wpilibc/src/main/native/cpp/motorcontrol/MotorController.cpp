// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/motor/MotorController.hpp"

#include "wpi/system/RobotController.hpp"

using namespace wpi;

void MotorController::SetVoltage(wpi::units::volt_t output) {
  // NOLINTNEXTLINE(bugprone-integer-division)
  Set(output / RobotController::GetBatteryVoltage());
}

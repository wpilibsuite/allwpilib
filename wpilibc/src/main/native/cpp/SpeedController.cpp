// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/SpeedController.h"

#include <wpi/sendable/SendableBuilder.h>

#include "frc/RobotController.h"

using namespace frc;

void SpeedController::SetVoltage(units::volt_t output) {
  Set(output / units::volt_t(RobotController::GetInputVoltage()));
}

void SpeedController::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Motor Controller");
  builder.SetActuator(true);
  builder.SetSafeState([=] { Disable(); });
  builder.AddDoubleProperty(
      "Value", [=] { return Get(); }, [=](double value) { Set(value); });
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/GyroBase.h"

#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

void GyroBase::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Gyro");
  builder.AddDoubleProperty(
      "Value", [=]() { return GetAngle(); }, nullptr);
}

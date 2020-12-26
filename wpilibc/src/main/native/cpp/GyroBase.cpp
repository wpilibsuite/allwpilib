// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/GyroBase.h"

#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

double GyroBase::PIDGet() {
  switch (GetPIDSourceType()) {
    case PIDSourceType::kRate:
      return GetRate();
    case PIDSourceType::kDisplacement:
      return GetAngle();
    default:
      return 0;
  }
}

void GyroBase::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Gyro");
  builder.AddDoubleProperty(
      "Value", [=]() { return GetAngle(); }, nullptr);
}

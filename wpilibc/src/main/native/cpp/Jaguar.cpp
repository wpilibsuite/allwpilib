// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Jaguar.h"

#include <hal/FRCUsageReporting.h>

#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

Jaguar::Jaguar(int channel) : PWMSpeedController(channel) {
  SetBounds(2.31, 1.55, 1.507, 1.454, 0.697);
  SetPeriodMultiplier(kPeriodMultiplier_1X);
  SetSpeed(0.0);
  SetZeroLatch();

  HAL_Report(HALUsageReporting::kResourceType_Jaguar, GetChannel() + 1);
  SendableRegistry::GetInstance().SetName(this, "Jaguar", GetChannel());
}

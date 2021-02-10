// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Talon.h"

#include <hal/FRCUsageReporting.h>

#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

Talon::Talon(int channel) : PWMSpeedController(channel) {
  SetBounds(2.037, 1.539, 1.513, 1.487, 0.989);
  SetPeriodMultiplier(kPeriodMultiplier_1X);
  SetSpeed(0.0);
  SetZeroLatch();

  HAL_Report(HALUsageReporting::kResourceType_Talon, GetChannel() + 1);
  SendableRegistry::GetInstance().SetName(this, "Talon", GetChannel());
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PWMVictorSPX.h"

#include <hal/FRCUsageReporting.h>

#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

PWMVictorSPX::PWMVictorSPX(int channel) : PWMSpeedController(channel) {
  SetBounds(2.004, 1.52, 1.50, 1.48, 0.997);
  SetPeriodMultiplier(kPeriodMultiplier_1X);
  SetSpeed(0.0);
  SetZeroLatch();

  HAL_Report(HALUsageReporting::kResourceType_PWMVictorSPX, GetChannel() + 1);
  SendableRegistry::GetInstance().SetName(this, "PWMVictorSPX", GetChannel());
}

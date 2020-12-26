// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PWMTalonSRX.h"

#include <hal/FRCUsageReporting.h>

#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

PWMTalonSRX::PWMTalonSRX(int channel) : PWMSpeedController(channel) {
  SetBounds(2.004, 1.52, 1.50, 1.48, 0.997);
  SetPeriodMultiplier(kPeriodMultiplier_1X);
  SetSpeed(0.0);
  SetZeroLatch();

  HAL_Report(HALUsageReporting::kResourceType_PWMTalonSRX, GetChannel() + 1);
  SendableRegistry::GetInstance().SetName(this, "PWMTalonSRX", GetChannel());
}

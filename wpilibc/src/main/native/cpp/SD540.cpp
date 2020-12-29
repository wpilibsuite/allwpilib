// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/SD540.h"

#include <hal/FRCUsageReporting.h>

#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

SD540::SD540(int channel) : PWMSpeedController(channel) {
  SetBounds(2.05, 1.55, 1.50, 1.44, 0.94);
  SetPeriodMultiplier(kPeriodMultiplier_1X);
  SetSpeed(0.0);
  SetZeroLatch();

  HAL_Report(HALUsageReporting::kResourceType_MindsensorsSD540,
             GetChannel() + 1);
  SendableRegistry::GetInstance().SetName(this, "SD540", GetChannel());
}

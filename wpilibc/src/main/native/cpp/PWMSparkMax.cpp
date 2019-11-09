/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/PWMSparkMax.h"

#include <hal/FRCUsageReporting.h>

#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

PWMSparkMax::PWMSparkMax(int channel) : PWMSpeedController(channel) {
  SetBounds(2.003, 1.55, 1.50, 1.46, .999);
  SetPeriodMultiplier(kPeriodMultiplier_1X);
  SetSpeed(0.0);
  SetZeroLatch();

  HAL_Report(HALUsageReporting::kResourceType_RevSparkMaxPWM, GetChannel() + 1);
  SendableRegistry::GetInstance().SetName(this, "PWMSparkMax", GetChannel());
}

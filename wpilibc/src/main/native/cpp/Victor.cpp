/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Victor.h"

#include <hal/FRCUsageReporting.h>

#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

Victor::Victor(int channel) : PWMSpeedController(channel) {
  SetBounds(2.027, 1.525, 1.507, 1.49, 1.026);
  SetPeriodMultiplier(kPeriodMultiplier_2X);
  SetSpeed(0.0);
  SetZeroLatch();

  HAL_Report(HALUsageReporting::kResourceType_Victor, GetChannel() + 1);
  SendableRegistry::GetInstance().SetName(this, "Victor", GetChannel());
}

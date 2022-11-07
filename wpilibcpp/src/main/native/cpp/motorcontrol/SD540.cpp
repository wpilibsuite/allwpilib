// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/motorcontrol/SD540.h"

#include <hal/FRCUsageReporting.h>

using namespace frc;

SD540::SD540(int channel) : PWMMotorController("SD540", channel) {
  m_pwm.SetBounds(2.05, 1.55, 1.50, 1.44, 0.94);
  m_pwm.SetPeriodMultiplier(PWM::kPeriodMultiplier_1X);
  m_pwm.SetSpeed(0.0);
  m_pwm.SetZeroLatch();

  HAL_Report(HALUsageReporting::kResourceType_MindsensorsSD540,
             GetChannel() + 1);
}

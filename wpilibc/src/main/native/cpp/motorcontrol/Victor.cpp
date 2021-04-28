// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/motorcontrol/Victor.h"

#include <hal/FRCUsageReporting.h>

using namespace frc;

Victor::Victor(int channel) : PWMMotorController("Victor", channel) {
  m_pwm.SetBounds(2.027, 1.525, 1.507, 1.49, 1.026);
  m_pwm.SetPeriodMultiplier(PWM::kPeriodMultiplier_2X);
  m_pwm.SetSpeed(0.0);
  m_pwm.SetZeroLatch();

  HAL_Report(HALUsageReporting::kResourceType_Victor, GetChannel() + 1);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/motorcontrol/Talon.h"

#include <hal/FRCUsageReporting.h>

using namespace frc;

Talon::Talon(int channel) : PWMMotorController("Talon", channel) {
  m_pwm.SetBounds(2.037, 1.539, 1.513, 1.487, 0.989);
  m_pwm.SetPeriodMultiplier(PWM::kPeriodMultiplier_1X);
  m_pwm.SetSpeed(0.0);
  m_pwm.SetZeroLatch();

  HAL_Report(HALUsageReporting::kResourceType_Talon, GetChannel() + 1);
}

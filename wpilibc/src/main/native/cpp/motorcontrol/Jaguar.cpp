// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/motorcontrol/Jaguar.h"

#include <hal/FRCUsageReporting.h>

using namespace frc;

Jaguar::Jaguar(int channel) : PWMMotorController("Jaguar", channel) {
  m_pwm.SetBounds(2.31, 1.55, 1.507, 1.454, 0.697);
  m_pwm.SetPeriodMultiplier(PWM::kPeriodMultiplier_1X);
  m_pwm.SetSpeed(0.0);
  m_pwm.SetZeroLatch();

  HAL_Report(HALUsageReporting::kResourceType_Jaguar, GetChannel() + 1);
}

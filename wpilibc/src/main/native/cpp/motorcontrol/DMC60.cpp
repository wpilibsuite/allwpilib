// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/motorcontrol/DMC60.h"

#include <hal/FRCUsageReporting.h>

using namespace frc;

DMC60::DMC60(int channel) : PWMMotorController("DMC60", channel) {
  m_pwm.SetBounds(2.004, 1.52, 1.50, 1.48, 0.997);
  m_pwm.SetPeriodMultiplier(PWM::kPeriodMultiplier_1X);
  m_pwm.SetSpeed(0.0);
  m_pwm.SetZeroLatch();

  HAL_Report(HALUsageReporting::kResourceType_DigilentDMC60, GetChannel() + 1);
}

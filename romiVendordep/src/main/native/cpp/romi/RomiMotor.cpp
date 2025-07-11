// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/romi/RomiMotor.h>

using namespace frc;

RomiMotor::RomiMotor(int channel) : PWMMotorController(channel) {
  m_pwm.SetOutputPeriod(PWM::kOutputPeriod_5Ms);
  SetSpeed(0.0);
}

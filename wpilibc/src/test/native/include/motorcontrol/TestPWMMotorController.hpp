// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hardware/motor/PWMMotorController.hpp"

namespace wpi {

class TestPWMMotorController : public PWMMotorController {
 public:
  explicit TestPWMMotorController(int channel) : PWMMotorController{channel} {
    SetBounds(2.003_ms, 1.55_ms, 1.5_ms, 1.46_ms, 0.999_ms);
    m_pwm.SetOutputPeriod(5_ms);
    SetThrottle(0.0);
  }
};

}  // namespace wpi

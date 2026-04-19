// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hardware/motor/MotorController.hpp"

namespace wpi {

class MockMotorController : public MotorController {
 public:
  void SetThrottle(double throttle) override;
  double GetThrottle() const override;
  void SetInverted(bool isInverted) override;
  bool GetInverted() const override;
  void Disable() override;

 private:
  double m_throttle = 0.0;
  bool m_isInverted = false;
};

}  // namespace wpi

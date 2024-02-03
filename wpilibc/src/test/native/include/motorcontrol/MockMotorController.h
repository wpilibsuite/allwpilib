// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/deprecated.h>

#include "frc/motorcontrol/MotorController.h"

namespace frc {

WPI_IGNORE_DEPRECATED

class MockMotorController : public MotorController {
 public:
  void Set(double speed) override;
  double Get() const override;
  void SetInverted(bool isInverted) override;
  bool GetInverted() const override;
  void Disable() override;
  void StopMotor() override;

 private:
  double m_speed = 0.0;
  bool m_isInverted = false;
};

WPI_UNIGNORE_DEPRECATED

}  // namespace frc

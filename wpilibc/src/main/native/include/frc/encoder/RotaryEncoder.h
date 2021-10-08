// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/angle.h>
#include <units/angular_velocity.h>

namespace frc {

class RotaryEncoder {
 public:
  virtual units::radian_t GetAngle() const = 0;
  virtual units::radians_per_second_t GetAngularVelocity() const = 0;
  virtual void SetInverted(bool inverted) = 0;
  virtual void Reset() = 0;
};

}  // namespace frc

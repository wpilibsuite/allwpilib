// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/length.h>
#include <units/velocity.h>

namespace frc {

class LinearEncoder {
 public:
  virtual units::meter_t GetDisplacement() const = 0;
  virtual units::meters_per_second_t GetVelocity() const = 0;
  virtual void SetInverted(bool inverted) = 0;
  virtual void Reset() = 0;
};

}  // namespace frc

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/MathUtil.h"

#include <cmath>

namespace frc {

double ApplyDeadband(double value, double deadband) {
  if (std::abs(value) > deadband) {
    if (value > 0.0) {
      return (value - deadband) / (1.0 - deadband);
    } else {
      return (value + deadband) / (1.0 - deadband);
    }
  } else {
    return 0.0;
  }
}

}  // namespace frc

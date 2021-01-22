// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/Base.h"

namespace frc {

/**
 * PIDOutput interface is a generic output for the PID class.
 *
 * PWMs use this class. Users implement this interface to allow for a
 * PIDController to read directly from the inputs.
 */
class PIDOutput {
 public:
  virtual void PIDWrite(double output) = 0;
};

}  // namespace frc

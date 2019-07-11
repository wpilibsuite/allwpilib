/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

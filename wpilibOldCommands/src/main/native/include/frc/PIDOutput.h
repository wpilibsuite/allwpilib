// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace frc {

/**
 * PIDOutput interface is a generic output for the PID class.
 *
 * MotorControllers use this class. Users implement this interface to allow for
 * a PIDController to read directly from the inputs.
 *
 * This class is provided by the OldCommands VendorDep
 */
class PIDOutput {
 public:
  virtual void PIDWrite(double output) = 0;
};

}  // namespace frc

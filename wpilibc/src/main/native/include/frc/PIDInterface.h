// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/deprecated.h>

namespace frc {

/**
 * Interface for PID Control Loop.
 *
 * @deprecated All APIs which use this have been deprecated.
 */
class PIDInterface {
 public:
  WPI_DEPRECATED("All APIs which use this have been deprecated.")
  PIDInterface() = default;
  PIDInterface(PIDInterface&&) = default;
  PIDInterface& operator=(PIDInterface&&) = default;

  virtual void SetPID(double p, double i, double d) = 0;
  virtual double GetP() const = 0;
  virtual double GetI() const = 0;
  virtual double GetD() const = 0;

  virtual void SetSetpoint(double setpoint) = 0;
  virtual double GetSetpoint() const = 0;

  virtual void Reset() = 0;
};

}  // namespace frc

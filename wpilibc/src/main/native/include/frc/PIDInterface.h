/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/deprecated.h>

namespace frc {

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

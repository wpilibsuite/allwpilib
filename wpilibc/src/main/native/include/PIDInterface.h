/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "Base.h"
#include "Controller.h"
#include "LiveWindow/LiveWindow.h"

namespace frc {

class PIDInterface : public Controller {
  virtual void SetPID(double p, double i, double d) = 0;
  virtual double GetP() const = 0;
  virtual double GetI() const = 0;
  virtual double GetD() const = 0;

  virtual void SetSetpoint(double setpoint) = 0;
  virtual double GetSetpoint() const = 0;

  virtual void Enable() = 0;
  virtual void Disable() = 0;
  virtual bool IsEnabled() const = 0;

  virtual void Reset() = 0;
};

}  // namespace frc

#pragma once

#include "Base.h"
#include "Controller.h"
#include "LiveWindow/LiveWindow.h"

class PIDInterface : public Controller {
  virtual void SetPID(double p, double i, double d) = 0;
  virtual double GetP() const = 0;
  virtual double GetI() const = 0;
  virtual double GetD() const = 0;

  virtual void SetSetpoint(float setpoint) = 0;
  virtual double GetSetpoint() const = 0;

  virtual void Enable() = 0;
  virtual void Disable() = 0;
  virtual bool IsEnabled() const = 0;

  virtual void Reset() = 0;
};

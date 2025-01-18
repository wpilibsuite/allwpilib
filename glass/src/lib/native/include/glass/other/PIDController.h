// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "glass/Model.h"

namespace glass {
class DoubleSource;
class PIDControllerModel : public Model {
 public:
  virtual const char* GetName() const = 0;

  virtual DoubleSource* GetPData() = 0;
  virtual DoubleSource* GetIData() = 0;
  virtual DoubleSource* GetDData() = 0;
  virtual DoubleSource* GetSetpointData() = 0;
  virtual DoubleSource* GetIZoneData() = 0;

  virtual void SetP(double value) = 0;
  virtual void SetI(double value) = 0;
  virtual void SetD(double value) = 0;
  virtual void SetSetpoint(double value) = 0;
  virtual void SetIZone(double value) = 0;
};
void DisplayPIDController(PIDControllerModel* m);
}  // namespace glass

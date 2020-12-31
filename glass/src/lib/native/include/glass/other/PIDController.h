// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "glass/Model.h"

namespace glass {
class DataSource;
class PIDControllerModel : public Model {
 public:
  virtual const char* GetName() const = 0;

  virtual DataSource* GetPData() = 0;
  virtual DataSource* GetIData() = 0;
  virtual DataSource* GetDData() = 0;
  virtual DataSource* GetSetpointData() = 0;

  virtual void SetP(double value) = 0;
  virtual void SetI(double value) = 0;
  virtual void SetD(double value) = 0;
  virtual void SetSetpoint(double value) = 0;
};
void DisplayPIDController(PIDControllerModel* m);
}  // namespace glass

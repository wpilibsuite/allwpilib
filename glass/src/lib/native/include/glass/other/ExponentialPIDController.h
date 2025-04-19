// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "glass/Model.h"

namespace glass {
class DataSource;
class ExponentialPIDControllerModel : public Model {
 public:
  virtual const char* GetName() const = 0;

  virtual DataSource* GetPData() = 0;
  virtual DataSource* GetIData() = 0;
  virtual DataSource* GetDData() = 0;
  virtual DataSource* GetIZoneData() = 0;
  virtual DataSource* GetMaxInputData() = 0;
  virtual DataSource* GetAData() = 0;
  virtual DataSource* GetBData() = 0;
  virtual DataSource* GetGoalData() = 0;

  virtual void SetP(double value) = 0;
  virtual void SetI(double value) = 0;
  virtual void SetD(double value) = 0;
  virtual void SetIZone(double value) = 0;
  virtual void SetMaxInput(double value) = 0;
  virtual void SetA(double value) = 0;
  virtual void SetB(double value) = 0;
  virtual void SetGoal(double value) = 0;
};
void DisplayExponentialPIDController(ExponentialPIDControllerModel* m);
}  // namespace glass

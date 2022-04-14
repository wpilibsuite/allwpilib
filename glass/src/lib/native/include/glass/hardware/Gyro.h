// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "glass/Model.h"

namespace glass {
class DataSource;
class GyroModel : public Model {
 public:
  virtual const char* GetName() const = 0;
  virtual const char* GetSimDevice() const = 0;

  virtual DataSource* GetAngleData() = 0;
  virtual void SetAngle(double angle) = 0;
};
void DisplayGyro(GyroModel* m);
}  // namespace glass

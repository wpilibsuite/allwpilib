// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "glass/Model.h"

namespace glass {

class DoubleSource;

class AccelerometerModel : public Model {
 public:
  virtual DoubleSource* GetXData() = 0;
  virtual DoubleSource* GetYData() = 0;
  virtual DoubleSource* GetZData() = 0;

  virtual int GetRange() = 0;

  virtual void SetX(double val) = 0;
  virtual void SetY(double val) = 0;
  virtual void SetZ(double val) = 0;
  virtual void SetRange(int val) = 0;
};

void DisplayAccelerometerDevice(AccelerometerModel* model);

}  // namespace glass

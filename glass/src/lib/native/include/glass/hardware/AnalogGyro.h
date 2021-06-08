// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/function_ref.h>

#include "glass/Model.h"

namespace glass {

class DataSource;

class AnalogGyroModel : public Model {
 public:
  virtual DataSource* GetAngleData() = 0;
  virtual DataSource* GetRateData() = 0;

  virtual void SetAngle(double val) = 0;
  virtual void SetRate(double val) = 0;
};

class AnalogGyrosModel : public Model {
 public:
  virtual void ForEachAnalogGyro(
      wpi::function_ref<void(AnalogGyroModel& model, int index)> func) = 0;
};

void DisplayAnalogGyroDevice(AnalogGyroModel* model, int index);
void DisplayAnalogGyrosDevice(AnalogGyrosModel* model);

}  // namespace glass

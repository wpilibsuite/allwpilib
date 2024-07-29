// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/function_ref.h>

#include "glass/Model.h"

namespace glass {

class DataSource;

class AnalogOutputModel : public Model {
 public:
  virtual DataSource* GetVoltageData() = 0;

  virtual void SetVoltage(double val) = 0;
};

class AnalogOutputsModel : public Model {
 public:
  virtual void ForEachAnalogOutput(
      wpi::function_ref<void(AnalogOutputModel& model, int index)> func) = 0;
};

void DisplayAnalogOutputsDevice(AnalogOutputsModel* model);

}  // namespace glass

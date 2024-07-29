// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <wpi/function_ref.h>

#include "glass/Model.h"

namespace glass {

class DataSource;

class AnalogInputModel : public Model {
 public:
  virtual bool IsGyro() const = 0;
  virtual const char* GetSimDevice() const = 0;

  virtual DataSource* GetVoltageData() = 0;

  virtual void SetVoltage(double val) = 0;
};

class AnalogInputsModel : public Model {
 public:
  virtual void ForEachAnalogInput(
      wpi::function_ref<void(AnalogInputModel& model, int index)> func) = 0;
};

void DisplayAnalogInput(AnalogInputModel* model, int index);
void DisplayAnalogInputs(AnalogInputsModel* model,
                         std::string_view noneMsg = "No analog inputs");

}  // namespace glass

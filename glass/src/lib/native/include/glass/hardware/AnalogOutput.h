/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/STLExtras.h>

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

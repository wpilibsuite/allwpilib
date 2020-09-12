/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/STLExtras.h>
#include <wpi/StringRef.h>

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
                         wpi::StringRef noneMsg = "No analog inputs");

}  // namespace glass

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

class PDPModel : public Model {
 public:
  virtual int GetNumChannels() const = 0;

  virtual DataSource* GetTemperatureData() = 0;
  virtual DataSource* GetVoltageData() = 0;
  virtual DataSource* GetCurrentData(int channel) = 0;

  virtual void SetTemperature(double val) = 0;
  virtual void SetVoltage(double val) = 0;
  virtual void SetCurrent(int channel, double val) = 0;
};

class PDPsModel : public Model {
 public:
  virtual void ForEachPDP(
      wpi::function_ref<void(PDPModel& model, int index)> func) = 0;
};

void DisplayPDP(PDPModel* model, int index);
void DisplayPDPs(PDPsModel* model, wpi::StringRef noneMsg = "No PDPs");

}  // namespace glass

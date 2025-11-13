// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "wpi/glass/Model.hpp"
#include "wpi/util/function_ref.hpp"

namespace wpi::glass {

class DoubleSource;

class PowerDistributionModel : public Model {
 public:
  virtual int GetNumChannels() const = 0;

  virtual DoubleSource* GetTemperatureData() = 0;
  virtual DoubleSource* GetVoltageData() = 0;
  virtual DoubleSource* GetCurrentData(int channel) = 0;

  virtual void SetTemperature(double val) = 0;
  virtual void SetVoltage(double val) = 0;
  virtual void SetCurrent(int channel, double val) = 0;
};

class PowerDistributionsModel : public Model {
 public:
  virtual void ForEachPowerDistribution(
      wpi::util::function_ref<void(PowerDistributionModel& model, int index)>
          func) = 0;
};

void DisplayPowerDistribution(PowerDistributionModel* model, int index);
void DisplayPowerDistributions(
    PowerDistributionsModel* model,
    std::string_view noneMsg = "No Power Distributions");

}  // namespace wpi::glass

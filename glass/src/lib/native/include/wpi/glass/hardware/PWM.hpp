// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "wpi/glass/Model.hpp"
#include "wpi/util/function_ref.hpp"

namespace wpi::glass {

class DoubleSource;

class PWMModel : public Model {
 public:
  virtual DoubleSource* GetSpeedData() = 0;

  virtual void SetSpeed(double val) = 0;
};

class PWMsModel : public Model {
 public:
  virtual void ForEachPWM(
      wpi::util::function_ref<void(PWMModel& model, int index)> func) = 0;
};

void DisplayPWM(PWMModel* model, int index, bool outputsEnabled);
void DisplayPWMs(PWMsModel* model, bool outputsEnabled,
                 std::string_view noneMsg = "No PWM outputs");

}  // namespace wpi::glass

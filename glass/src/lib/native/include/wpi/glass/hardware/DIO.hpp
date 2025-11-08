// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "wpi/glass/Model.hpp"
#include "wpi/util/function_ref.hpp"

namespace glass {

class EncoderModel;
class BooleanSource;
class DoubleSource;

class DPWMModel : public Model {
 public:
  virtual const char* GetSimDevice() const = 0;

  virtual DoubleSource* GetValueData() = 0;

  virtual void SetValue(double val) = 0;
};

class DutyCycleModel : public Model {
 public:
  virtual const char* GetSimDevice() const = 0;

  virtual DoubleSource* GetValueData() = 0;

  virtual void SetValue(double val) = 0;
};

class DIOModel : public Model {
 public:
  virtual const char* GetName() const = 0;

  virtual const char* GetSimDevice() const = 0;

  virtual DPWMModel* GetDPWM() = 0;
  virtual DutyCycleModel* GetDutyCycle() = 0;
  virtual EncoderModel* GetEncoder() = 0;

  virtual bool IsInput() const = 0;

  virtual BooleanSource* GetValueData() = 0;

  virtual void SetValue(bool val) = 0;
};

class DIOsModel : public Model {
 public:
  virtual void ForEachDIO(
      wpi::function_ref<void(DIOModel& model, int index)> func) = 0;
};

void DisplayDIO(DIOModel* model, int index, bool outputsEnabled);
void DisplayDIOs(DIOsModel* model, bool outputsEnabled,
                 std::string_view noneMsg = "No Digital I/O");

}  // namespace glass

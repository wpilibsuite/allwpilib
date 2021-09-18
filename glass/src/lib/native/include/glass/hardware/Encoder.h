// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <wpi/function_ref.h>

#include "glass/Model.h"

namespace glass {

class DataSource;

class EncoderModel : public Model {
 public:
  virtual void SetName(std::string_view name);

  virtual const char* GetSimDevice() const = 0;

  virtual int GetChannelA() const = 0;
  virtual int GetChannelB() const = 0;

  virtual DataSource* GetDistancePerPulseData() = 0;
  virtual DataSource* GetCountData() = 0;
  virtual DataSource* GetPeriodData() = 0;
  virtual DataSource* GetDirectionData() = 0;
  virtual DataSource* GetDistanceData() = 0;
  virtual DataSource* GetRateData() = 0;

  virtual double GetMaxPeriod() = 0;
  virtual bool GetReverseDirection() = 0;

  virtual void SetDistancePerPulse(double val) = 0;
  virtual void SetCount(int val) = 0;
  virtual void SetPeriod(double val) = 0;
  virtual void SetDirection(bool val) = 0;
  virtual void SetDistance(double val) = 0;
  virtual void SetRate(double val) = 0;

  virtual void SetMaxPeriod(double val) = 0;
  virtual void SetReverseDirection(bool val) = 0;
};

class EncodersModel : public Model {
 public:
  virtual void ForEachEncoder(
      wpi::function_ref<void(EncoderModel& model, int index)> func) = 0;
};

void DisplayEncoder(EncoderModel* model);
void DisplayEncoders(EncodersModel* model,
                     std::string_view noneMsg = "No encoders");

}  // namespace glass

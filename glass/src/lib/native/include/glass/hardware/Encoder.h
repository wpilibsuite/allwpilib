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

class EncoderModel : public Model {
 public:
  virtual void SetName(const wpi::Twine& name);

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
                     wpi::StringRef noneMsg = "No encoders");

}  // namespace glass

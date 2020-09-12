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

class EncoderModel;
class DataSource;

class DPWMModel : public Model {
 public:
  virtual const char* GetSimDevice() const = 0;

  virtual DataSource* GetValueData() = 0;

  virtual void SetValue(double val) = 0;
};

class DutyCycleModel : public Model {
 public:
  virtual const char* GetSimDevice() const = 0;

  virtual DataSource* GetValueData() = 0;

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

  virtual DataSource* GetValueData() = 0;

  virtual void SetValue(bool val) = 0;
};

class DIOsModel : public Model {
 public:
  virtual void ForEachDIO(
      wpi::function_ref<void(DIOModel& model, int index)> func) = 0;
};

void DisplayDIO(DIOModel* model, int index, bool outputsEnabled);
void DisplayDIOs(DIOsModel* model, bool outputsEnabled,
                 wpi::StringRef noneMsg = "No Digital I/O");

}  // namespace glass

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

class PWMModel : public Model {
 public:
  // returns -1 if not an addressable LED
  virtual int GetAddressableLED() const = 0;

  virtual DataSource* GetSpeedData() = 0;

  virtual void SetSpeed(double val) = 0;
};

class PWMsModel : public Model {
 public:
  virtual void ForEachPWM(
      wpi::function_ref<void(PWMModel& model, int index)> func) = 0;
};

void DisplayPWM(PWMModel* model, int index, bool outputsEnabled);
void DisplayPWMs(PWMsModel* model, bool outputsEnabled,
                 wpi::StringRef noneMsg = "No PWM outputs");

}  // namespace glass

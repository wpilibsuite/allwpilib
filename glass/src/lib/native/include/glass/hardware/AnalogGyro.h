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

class AnalogGyroModel : public Model {
 public:
  virtual DataSource* GetAngleData() = 0;
  virtual DataSource* GetRateData() = 0;

  virtual void SetAngle(double val) = 0;
  virtual void SetRate(double val) = 0;
};

class AnalogGyrosModel : public Model {
 public:
  virtual void ForEachAnalogGyro(
      wpi::function_ref<void(AnalogGyroModel& model, int index)> func) = 0;
};

void DisplayAnalogGyroDevice(AnalogGyroModel* model, int index);
void DisplayAnalogGyrosDevice(AnalogGyrosModel* model);

}  // namespace glass

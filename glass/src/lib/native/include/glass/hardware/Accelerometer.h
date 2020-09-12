/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "glass/Model.h"

namespace glass {

class DataSource;

class AccelerometerModel : public Model {
 public:
  virtual DataSource* GetXData() = 0;
  virtual DataSource* GetYData() = 0;
  virtual DataSource* GetZData() = 0;

  virtual int GetRange() = 0;

  virtual void SetX(double val) = 0;
  virtual void SetY(double val) = 0;
  virtual void SetZ(double val) = 0;
  virtual void SetRange(int val) = 0;
};

void DisplayAccelerometerDevice(AccelerometerModel* model);

}  // namespace glass

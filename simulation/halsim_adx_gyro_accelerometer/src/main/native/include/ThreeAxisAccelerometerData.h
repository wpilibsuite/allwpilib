/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <mockdata/SimDataValue.h>

namespace hal {
class ThreeAxisAccelerometerData {
  HAL_SIMDATAVALUE_DEFINE_NAME(X);
  HAL_SIMDATAVALUE_DEFINE_NAME(Y);
  HAL_SIMDATAVALUE_DEFINE_NAME(Z);

 public:
  ThreeAxisAccelerometerData();
  virtual ~ThreeAxisAccelerometerData();

  virtual bool GetInitialized() const = 0;

  double GetX() { return x; }
  void SetX(double x_) { x = x_; }

  double GetY() { return y; }
  void SetY(double y_) { y = y_; }

  double GetZ() { return z; }
  void SetZ(double z_) { z = z_; }

  SimDataValue<double, HAL_MakeDouble, GetXName> x{0.0};
  SimDataValue<double, HAL_MakeDouble, GetYName> y{0.0};
  SimDataValue<double, HAL_MakeDouble, GetZName> z{0.0};

  virtual void ResetData();
};
}  // namespace hal

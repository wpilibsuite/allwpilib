/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "mockdata/AccelerometerData.h"
#include "mockdata/SimDataValue.h"

namespace hal {
class AccelerometerData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Active)
  HAL_SIMDATAVALUE_DEFINE_NAME(Range)
  HAL_SIMDATAVALUE_DEFINE_NAME(X)
  HAL_SIMDATAVALUE_DEFINE_NAME(Y)
  HAL_SIMDATAVALUE_DEFINE_NAME(Z)

  static inline HAL_Value MakeRangeValue(HAL_AccelerometerRange value) {
    return MakeEnum(value);
  }

 public:
  SimDataValue<HAL_Bool, MakeBoolean, GetActiveName> active{false};
  SimDataValue<HAL_AccelerometerRange, MakeRangeValue, GetRangeName> range{
      static_cast<HAL_AccelerometerRange>(0)};
  SimDataValue<double, MakeDouble, GetXName> x{0.0};
  SimDataValue<double, MakeDouble, GetYName> y{0.0};
  SimDataValue<double, MakeDouble, GetZName> z{0.0};

  virtual void ResetData();
};
extern AccelerometerData* SimAccelerometerData;
}  // namespace hal

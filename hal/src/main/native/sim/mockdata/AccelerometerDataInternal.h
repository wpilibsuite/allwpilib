// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/simulation/AccelerometerData.h"
#include "hal/simulation/SimDataValue.h"

namespace hal {
class AccelerometerData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Active)
  HAL_SIMDATAVALUE_DEFINE_NAME(Range)
  HAL_SIMDATAVALUE_DEFINE_NAME(X)
  HAL_SIMDATAVALUE_DEFINE_NAME(Y)
  HAL_SIMDATAVALUE_DEFINE_NAME(Z)

  static inline HAL_Value MakeRangeValue(HAL_AccelerometerRange value) {
    return HAL_MakeEnum(value);
  }

 public:
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetActiveName> active{false};
  SimDataValue<HAL_AccelerometerRange, MakeRangeValue, GetRangeName> range{
      static_cast<HAL_AccelerometerRange>(0)};
  SimDataValue<double, HAL_MakeDouble, GetXName> x{0.0};
  SimDataValue<double, HAL_MakeDouble, GetYName> y{0.0};
  SimDataValue<double, HAL_MakeDouble, GetZName> z{0.0};

  virtual void ResetData();
};
extern AccelerometerData* SimAccelerometerData;
}  // namespace hal

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/simulation/SPIAccelerometerData.h"
#include "hal/simulation/SimDataValue.h"

namespace hal {
class SPIAccelerometerData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Active)
  HAL_SIMDATAVALUE_DEFINE_NAME(Range)
  HAL_SIMDATAVALUE_DEFINE_NAME(X)
  HAL_SIMDATAVALUE_DEFINE_NAME(Y)
  HAL_SIMDATAVALUE_DEFINE_NAME(Z)

 public:
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetActiveName> active{false};
  SimDataValue<int32_t, HAL_MakeInt, GetRangeName> range{0};
  SimDataValue<double, HAL_MakeDouble, GetXName> x{0.0};
  SimDataValue<double, HAL_MakeDouble, GetYName> y{0.0};
  SimDataValue<double, HAL_MakeDouble, GetZName> z{0.0};

  virtual void ResetData();
};
extern SPIAccelerometerData* SimSPIAccelerometerData;
}  // namespace hal

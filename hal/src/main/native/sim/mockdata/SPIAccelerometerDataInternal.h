/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "mockdata/SPIAccelerometerData.h"
#include "mockdata/SimDataValue.h"

namespace hal {
class SPIAccelerometerData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Active)
  HAL_SIMDATAVALUE_DEFINE_NAME(Range)
  HAL_SIMDATAVALUE_DEFINE_NAME(X)
  HAL_SIMDATAVALUE_DEFINE_NAME(Y)
  HAL_SIMDATAVALUE_DEFINE_NAME(Z)

 public:
  SimDataValue<HAL_Bool, MakeBoolean, GetActiveName> active{false};
  SimDataValue<int32_t, MakeInt, GetRangeName> range{0};
  SimDataValue<double, MakeDouble, GetXName> x{0.0};
  SimDataValue<double, MakeDouble, GetYName> y{0.0};
  SimDataValue<double, MakeDouble, GetZName> z{0.0};

  virtual void ResetData();
};
extern SPIAccelerometerData* SimSPIAccelerometerData;
}  // namespace hal

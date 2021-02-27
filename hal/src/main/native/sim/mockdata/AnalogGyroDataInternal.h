// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/simulation/AnalogGyroData.h"
#include "hal/simulation/SimDataValue.h"

namespace hal {
class AnalogGyroData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Angle)
  HAL_SIMDATAVALUE_DEFINE_NAME(Rate)
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)

 public:
  SimDataValue<double, HAL_MakeDouble, GetAngleName> angle{0.0};
  SimDataValue<double, HAL_MakeDouble, GetRateName> rate{0.0};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{
      false};

  virtual void ResetData();
};
extern AnalogGyroData* SimAnalogGyroData;
}  // namespace hal

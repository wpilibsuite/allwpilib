// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/simulation/DigitalPWMData.h"
#include "hal/simulation/SimDataValue.h"

namespace hal {
class DigitalPWMData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(DutyCycle)
  HAL_SIMDATAVALUE_DEFINE_NAME(Pin)

 public:
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{
      false};
  SimDataValue<double, HAL_MakeDouble, GetDutyCycleName> dutyCycle{0.0};
  SimDataValue<int32_t, HAL_MakeInt, GetPinName> pin{0};

  virtual void ResetData();
};
extern DigitalPWMData* SimDigitalPWMData;
}  // namespace hal

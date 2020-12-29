// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/simulation/AnalogOutData.h"
#include "hal/simulation/SimDataValue.h"

namespace hal {
class AnalogOutData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Voltage)
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)

 public:
  SimDataValue<double, HAL_MakeDouble, GetVoltageName> voltage{0.0};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{0};

  virtual void ResetData();
};
extern AnalogOutData* SimAnalogOutData;
}  // namespace hal

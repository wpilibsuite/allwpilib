// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/simulation/AnalogInData.h"
#include "hal/simulation/SimDataValue.h"

namespace hal {
class AnalogInData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(Voltage)

 public:
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{
      false};
  std::atomic<HAL_SimDeviceHandle> simDevice;
  SimDataValue<double, HAL_MakeDouble, GetVoltageName> voltage{0.0};

  virtual void ResetData();
};
extern AnalogInData* SimAnalogInData;
}  // namespace hal

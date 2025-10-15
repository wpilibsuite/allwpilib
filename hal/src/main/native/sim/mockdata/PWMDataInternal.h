// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/simulation/PWMData.h"
#include "wpi/hal/simulation/SimDataValue.h"

namespace wpi::hal {
class PWMData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(PulseMicrosecond)
  HAL_SIMDATAVALUE_DEFINE_NAME(OutputPeriod)

 public:
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{
      false};
  std::atomic<HAL_SimDeviceHandle> simDevice;
  SimDataValue<int32_t, HAL_MakeInt, GetPulseMicrosecondName> pulseMicrosecond{
      0};
  SimDataValue<int32_t, HAL_MakeInt, GetOutputPeriodName> outputPeriod{0};

  virtual void ResetData();
};
extern PWMData* SimPWMData;
}  // namespace wpi::hal

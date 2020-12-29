// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/simulation/DIOData.h"
#include "hal/simulation/SimDataValue.h"

namespace hal {
class DIOData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(Value)
  HAL_SIMDATAVALUE_DEFINE_NAME(PulseLength)
  HAL_SIMDATAVALUE_DEFINE_NAME(IsInput)
  HAL_SIMDATAVALUE_DEFINE_NAME(FilterIndex)

 public:
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{
      false};
  std::atomic<HAL_SimDeviceHandle> simDevice;
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetValueName> value{true};
  SimDataValue<double, HAL_MakeDouble, GetPulseLengthName> pulseLength{0.0};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetIsInputName> isInput{true};
  SimDataValue<int32_t, HAL_MakeInt, GetFilterIndexName> filterIndex{-1};

  virtual void ResetData();
};
extern DIOData* SimDIOData;
}  // namespace hal

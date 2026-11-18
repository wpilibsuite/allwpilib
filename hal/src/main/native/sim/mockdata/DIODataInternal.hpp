// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/simulation/SimDataValue.hpp"

namespace wpi::hal {
class DIOData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(Value)
  HAL_SIMDATAVALUE_DEFINE_NAME(IsInput)

 public:
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{
      false};
  std::atomic<HAL_SimDeviceHandle> simDevice;
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetValueName> value{true};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetIsInputName> isInput{true};

  virtual void ResetData();
};
extern DIOData* SimDIOData;
}  // namespace wpi::hal

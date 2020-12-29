// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/simulation/RelayData.h"
#include "hal/simulation/SimDataValue.h"

namespace hal {
class RelayData {
  HAL_SIMDATAVALUE_DEFINE_NAME(InitializedForward)
  HAL_SIMDATAVALUE_DEFINE_NAME(InitializedReverse)
  HAL_SIMDATAVALUE_DEFINE_NAME(Forward)
  HAL_SIMDATAVALUE_DEFINE_NAME(Reverse)

 public:
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedForwardName>
      initializedForward{false};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedReverseName>
      initializedReverse{false};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetForwardName> forward{false};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetReverseName> reverse{false};

  virtual void ResetData();
};
extern RelayData* SimRelayData;
}  // namespace hal

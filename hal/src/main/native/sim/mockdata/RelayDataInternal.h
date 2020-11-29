/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "hal/simulation/RelayData.h"
#include "hal/simulation/SimDataValue.h"
#include "hal/simulation/SimDisplayName.h"

namespace hal {
class RelayData {
  HAL_SIMDATAVALUE_DEFINE_NAME(InitializedForward)
  HAL_SIMDATAVALUE_DEFINE_NAME(InitializedReverse)
  HAL_SIMDATAVALUE_DEFINE_NAME(Forward)
  HAL_SIMDATAVALUE_DEFINE_NAME(Reverse)

 public:
  SimDisplayName displayName;
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

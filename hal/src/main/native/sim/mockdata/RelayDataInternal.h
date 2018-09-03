/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "mockdata/RelayData.h"
#include "mockdata/SimDataValue.h"

namespace hal {
class RelayData {
  HAL_SIMDATAVALUE_DEFINE_NAME(InitializedForward)
  HAL_SIMDATAVALUE_DEFINE_NAME(InitializedReverse)
  HAL_SIMDATAVALUE_DEFINE_NAME(Forward)
  HAL_SIMDATAVALUE_DEFINE_NAME(Reverse)

 public:
  SimDataValue<HAL_Bool, MakeBoolean, GetInitializedForwardName>
      initializedForward{false};
  SimDataValue<HAL_Bool, MakeBoolean, GetInitializedReverseName>
      initializedReverse{false};
  SimDataValue<HAL_Bool, MakeBoolean, GetForwardName> forward{false};
  SimDataValue<HAL_Bool, MakeBoolean, GetReverseName> reverse{false};

  virtual void ResetData();
};
extern RelayData* SimRelayData;
}  // namespace hal

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "hal/simulation/PWMData.h"
#include "hal/simulation/SimDataValue.h"
#include "hal/simulation/SimDisplayName.h"

namespace hal {
class PWMData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(RawValue)
  HAL_SIMDATAVALUE_DEFINE_NAME(Speed)
  HAL_SIMDATAVALUE_DEFINE_NAME(Position)
  HAL_SIMDATAVALUE_DEFINE_NAME(PeriodScale)
  HAL_SIMDATAVALUE_DEFINE_NAME(ZeroLatch)

 public:
  SimDisplayName displayName;
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{
      false};
  SimDataValue<int32_t, HAL_MakeInt, GetRawValueName> rawValue{0};
  SimDataValue<double, HAL_MakeDouble, GetSpeedName> speed{0};
  SimDataValue<double, HAL_MakeDouble, GetPositionName> position{0};
  SimDataValue<int32_t, HAL_MakeInt, GetPeriodScaleName> periodScale{0};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetZeroLatchName> zeroLatch{false};

  virtual void ResetData();
};
extern PWMData* SimPWMData;
}  // namespace hal

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <limits>

#include "hal/simulation/EncoderData.h"
#include "hal/simulation/SimDataValue.h"
#include "hal/simulation/SimDisplayName.h"

namespace hal {
class EncoderData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(Count)
  HAL_SIMDATAVALUE_DEFINE_NAME(Period)
  HAL_SIMDATAVALUE_DEFINE_NAME(Reset)
  HAL_SIMDATAVALUE_DEFINE_NAME(MaxPeriod)
  HAL_SIMDATAVALUE_DEFINE_NAME(Direction)
  HAL_SIMDATAVALUE_DEFINE_NAME(ReverseDirection)
  HAL_SIMDATAVALUE_DEFINE_NAME(SamplesToAverage)
  HAL_SIMDATAVALUE_DEFINE_NAME(DistancePerPulse)

 public:
  SimDisplayName displayName;
  std::atomic<int32_t> digitalChannelA{0};
  std::atomic<int32_t> digitalChannelB{0};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{
      false};
  std::atomic<HAL_SimDeviceHandle> simDevice;
  SimDataValue<int32_t, HAL_MakeInt, GetCountName> count{0};
  SimDataValue<double, HAL_MakeDouble, GetPeriodName> period{
      (std::numeric_limits<double>::max)()};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetResetName> reset{false};
  SimDataValue<double, HAL_MakeDouble, GetMaxPeriodName> maxPeriod{0};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetDirectionName> direction{false};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetReverseDirectionName>
      reverseDirection{false};
  SimDataValue<int32_t, HAL_MakeInt, GetSamplesToAverageName> samplesToAverage{
      0};
  SimDataValue<double, HAL_MakeDouble, GetDistancePerPulseName>
      distancePerPulse{1};

  virtual void ResetData();
};
extern EncoderData* SimEncoderData;
}  // namespace hal

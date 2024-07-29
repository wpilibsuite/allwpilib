// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <limits>

#include "hal/simulation/EncoderData.h"
#include "hal/simulation/SimDataValue.h"

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

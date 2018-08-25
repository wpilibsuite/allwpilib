/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <limits>

#include "mockdata/EncoderData.h"
#include "mockdata/SimDataValue.h"

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
  std::atomic<int16_t> digitalChannelA{0};
  SimDataValue<HAL_Bool, MakeBoolean, GetInitializedName> initialized{false};
  SimDataValue<int32_t, MakeInt, GetCountName> count{0};
  SimDataValue<double, MakeDouble, GetPeriodName> period{
      std::numeric_limits<double>::max()};
  SimDataValue<HAL_Bool, MakeBoolean, GetResetName> reset{false};
  SimDataValue<double, MakeDouble, GetMaxPeriodName> maxPeriod{0};
  SimDataValue<HAL_Bool, MakeBoolean, GetDirectionName> direction{false};
  SimDataValue<HAL_Bool, MakeBoolean, GetReverseDirectionName> reverseDirection{
      false};
  SimDataValue<int32_t, MakeInt, GetSamplesToAverageName> samplesToAverage{0};
  SimDataValue<double, MakeDouble, GetDistancePerPulseName> distancePerPulse{1};

  virtual void ResetData();
};
extern EncoderData* SimEncoderData;
}  // namespace hal

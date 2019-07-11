/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "mockdata/AnalogInData.h"
#include "mockdata/SimDataValue.h"

namespace hal {
class AnalogInData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(AverageBits)
  HAL_SIMDATAVALUE_DEFINE_NAME(OversampleBits)
  HAL_SIMDATAVALUE_DEFINE_NAME(Voltage)
  HAL_SIMDATAVALUE_DEFINE_NAME(AccumulatorInitialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(AccumulatorValue)
  HAL_SIMDATAVALUE_DEFINE_NAME(AccumulatorCount)
  HAL_SIMDATAVALUE_DEFINE_NAME(AccumulatorCenter)
  HAL_SIMDATAVALUE_DEFINE_NAME(AccumulatorDeadband)

 public:
  SimDataValue<HAL_Bool, MakeBoolean, GetInitializedName> initialized{false};
  SimDataValue<int32_t, MakeInt, GetAverageBitsName> averageBits{7};
  SimDataValue<int32_t, MakeInt, GetOversampleBitsName> oversampleBits{0};
  SimDataValue<double, MakeDouble, GetVoltageName> voltage{0.0};
  SimDataValue<HAL_Bool, MakeBoolean, GetAccumulatorInitializedName>
      accumulatorInitialized{false};
  SimDataValue<int64_t, MakeLong, GetAccumulatorValueName> accumulatorValue{0};
  SimDataValue<int64_t, MakeLong, GetAccumulatorCountName> accumulatorCount{0};
  SimDataValue<int32_t, MakeInt, GetAccumulatorCenterName> accumulatorCenter{0};
  SimDataValue<int32_t, MakeInt, GetAccumulatorDeadbandName>
      accumulatorDeadband{0};

  virtual void ResetData();
};
extern AnalogInData* SimAnalogInData;
}  // namespace hal

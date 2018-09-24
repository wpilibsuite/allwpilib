/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "mockdata/AnalogTriggerData.h"
#include "mockdata/SimDataValue.h"

namespace hal {
class AnalogTriggerData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(TriggerLowerBound)
  HAL_SIMDATAVALUE_DEFINE_NAME(TriggerUpperBound)
  HAL_SIMDATAVALUE_DEFINE_NAME(TriggerMode)

  static LLVM_ATTRIBUTE_ALWAYS_INLINE HAL_Value
  MakeTriggerModeValue(HALSIM_AnalogTriggerMode value) {
    return MakeEnum(value);
  }

 public:
  SimDataValue<HAL_Bool, MakeBoolean, GetInitializedName> initialized{0};
  SimDataValue<double, MakeDouble, GetTriggerLowerBoundName> triggerLowerBound{
      0};
  SimDataValue<double, MakeDouble, GetTriggerUpperBoundName> triggerUpperBound{
      0};
  SimDataValue<HALSIM_AnalogTriggerMode, MakeTriggerModeValue,
               GetTriggerModeName>
      triggerMode{static_cast<HALSIM_AnalogTriggerMode>(0)};

  virtual void ResetData();
};
extern AnalogTriggerData* SimAnalogTriggerData;
}  // namespace hal

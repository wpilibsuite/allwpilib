// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/simulation/AnalogTriggerData.h"
#include "hal/simulation/SimDataValue.h"

namespace hal {
class AnalogTriggerData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(TriggerLowerBound)
  HAL_SIMDATAVALUE_DEFINE_NAME(TriggerUpperBound)
  HAL_SIMDATAVALUE_DEFINE_NAME(TriggerMode)

  static LLVM_ATTRIBUTE_ALWAYS_INLINE HAL_Value
  MakeTriggerModeValue(HALSIM_AnalogTriggerMode value) {
    return HAL_MakeEnum(value);
  }

 public:
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{0};
  SimDataValue<double, HAL_MakeDouble, GetTriggerLowerBoundName>
      triggerLowerBound{0};
  SimDataValue<double, HAL_MakeDouble, GetTriggerUpperBoundName>
      triggerUpperBound{0};
  SimDataValue<HALSIM_AnalogTriggerMode, MakeTriggerModeValue,
               GetTriggerModeName>
      triggerMode{static_cast<HALSIM_AnalogTriggerMode>(0)};
  std::atomic<int32_t> inputPort;

  virtual void ResetData();
};
extern AnalogTriggerData* SimAnalogTriggerData;
}  // namespace hal

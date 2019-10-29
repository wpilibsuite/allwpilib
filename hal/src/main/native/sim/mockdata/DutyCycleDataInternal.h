/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <atomic>
#include <limits>

#include "mockdata/DutyCycleData.h"
#include "mockdata/SimDataValue.h"

namespace hal {
class DutyCycleData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(Output)
  HAL_SIMDATAVALUE_DEFINE_NAME(Frequency)

 public:
  std::atomic<int32_t> digitalChannel{0};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{
      false};
  std::atomic<HAL_SimDeviceHandle> simDevice;
  SimDataValue<int32_t, HAL_MakeInt, GetFrequencyName> frequency{0};
  SimDataValue<double, HAL_MakeDouble, GetOutputName> output{0};

  virtual void ResetData();
};
extern DutyCycleData* SimDutyCycleData;
}  // namespace hal

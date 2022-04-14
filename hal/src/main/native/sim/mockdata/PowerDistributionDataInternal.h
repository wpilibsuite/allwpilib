// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "../PortsInternal.h"
#include "hal/simulation/PowerDistributionData.h"
#include "hal/simulation/SimDataValue.h"

namespace hal {

class PowerDistributionData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(Temperature)
  HAL_SIMDATAVALUE_DEFINE_NAME(Voltage)
  HAL_SIMDATAVALUE_DEFINE_NAME(Current)

  static LLVM_ATTRIBUTE_ALWAYS_INLINE constexpr double GetCurrentDefault() {
    return 0.0;
  }

 public:
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{
      false};
  SimDataValue<double, HAL_MakeDouble, GetTemperatureName> temperature{0.0};
  SimDataValue<double, HAL_MakeDouble, GetVoltageName> voltage{12.0};
  SimDataValue<double, HAL_MakeDouble, GetCurrentName, GetCurrentDefault>
      current[kNumPDSimChannels];

  virtual void ResetData();
};
extern PowerDistributionData* SimPowerDistributionData;
}  // namespace hal

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "../PortsInternal.h"
#include "hal/simulation/PDPData.h"
#include "hal/simulation/SimDataValue.h"
#include "hal/simulation/SimDisplayName.h"

namespace hal {
class PDPData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(Temperature)
  HAL_SIMDATAVALUE_DEFINE_NAME(Voltage)
  HAL_SIMDATAVALUE_DEFINE_NAME(Current)

  static LLVM_ATTRIBUTE_ALWAYS_INLINE constexpr double GetCurrentDefault() {
    return 0.0;
  }

 public:
  SimDisplayName displayName;
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{
      false};
  SimDataValue<double, HAL_MakeDouble, GetTemperatureName> temperature{0.0};
  SimDataValue<double, HAL_MakeDouble, GetVoltageName> voltage{12.0};
  SimDataValue<double, HAL_MakeDouble, GetCurrentName, GetCurrentDefault>
      current[kNumPDPChannels];

  virtual void ResetData();
};
extern PDPData* SimPDPData;
}  // namespace hal

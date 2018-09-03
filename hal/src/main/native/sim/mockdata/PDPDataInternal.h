/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "../PortsInternal.h"
#include "mockdata/PDPData.h"
#include "mockdata/SimDataValue.h"

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
  SimDataValue<HAL_Bool, MakeBoolean, GetInitializedName> initialized{false};
  SimDataValue<double, MakeDouble, GetTemperatureName> temperature{0.0};
  SimDataValue<double, MakeDouble, GetVoltageName> voltage{12.0};
  SimDataValue<double, MakeDouble, GetCurrentName, GetCurrentDefault>
      current[kNumPDPChannels];

  virtual void ResetData();
};
extern PDPData* SimPDPData;
}  // namespace hal

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "../PortsInternal.h"
#include "hal/simulation/CTREPCMData.h"
#include "hal/simulation/SimDataValue.h"

namespace hal {
class CTREPCMData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(SolenoidOutput)
  HAL_SIMDATAVALUE_DEFINE_NAME(CompressorOn)
  HAL_SIMDATAVALUE_DEFINE_NAME(ClosedLoopEnabled)
  HAL_SIMDATAVALUE_DEFINE_NAME(PressureSwitch)
  HAL_SIMDATAVALUE_DEFINE_NAME(CompressorCurrent)

  static LLVM_ATTRIBUTE_ALWAYS_INLINE constexpr HAL_Bool
  GetSolenoidOutputDefault() {
    return false;
  }

 public:
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{
      false};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetSolenoidOutputName,
               GetSolenoidOutputDefault>
      solenoidOutput[kNumCTRESolenoidChannels];
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetCompressorOnName> compressorOn{
      false};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetClosedLoopEnabledName>
      closedLoopEnabled{true};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetPressureSwitchName> pressureSwitch{
      false};
  SimDataValue<double, HAL_MakeDouble, GetCompressorCurrentName>
      compressorCurrent{0.0};

  virtual void ResetData();
};
extern CTREPCMData* SimCTREPCMData;
}  // namespace hal

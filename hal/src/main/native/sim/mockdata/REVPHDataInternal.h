// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "../PortsInternal.h"
#include "hal/simulation/REVPHData.h"
#include "hal/simulation/SimDataValue.h"

namespace hal {
class REVPHData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMDATAVALUE_DEFINE_NAME(SolenoidOutput)
  HAL_SIMDATAVALUE_DEFINE_NAME(CompressorOn)
  HAL_SIMDATAVALUE_DEFINE_NAME(CompressorConfigType)
  HAL_SIMDATAVALUE_DEFINE_NAME(PressureSwitch)
  HAL_SIMDATAVALUE_DEFINE_NAME(CompressorCurrent)

  static LLVM_ATTRIBUTE_ALWAYS_INLINE constexpr HAL_Bool
  GetSolenoidOutputDefault() {
    return false;
  }

  static inline HAL_Value MakeCompressorConfigTypeValue(
      HAL_REVPHCompressorConfigType value) {
    return HAL_MakeEnum(value);
  }

 public:
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{
      false};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetSolenoidOutputName,
               GetSolenoidOutputDefault>
      solenoidOutput[kNumREVPHChannels];
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetCompressorOnName> compressorOn{
      false};
  SimDataValue<HAL_REVPHCompressorConfigType, MakeCompressorConfigTypeValue,
               GetCompressorConfigTypeName>
      compressorConfigType{HAL_REVPHCompressorConfigType::
                               HAL_REVPHCompressorConfigType_kDisabled};
  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetPressureSwitchName> pressureSwitch{
      false};
  SimDataValue<double, HAL_MakeDouble, GetCompressorCurrentName>
      compressorCurrent{0.0};

  virtual void ResetData();
};
extern REVPHData* SimREVPHData;
}  // namespace hal

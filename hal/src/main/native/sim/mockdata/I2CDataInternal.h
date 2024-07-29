// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/simulation/I2CData.h"
#include "hal/simulation/SimCallbackRegistry.h"
#include "hal/simulation/SimDataValue.h"

namespace hal {
class I2CData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(Read)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(Write)

 public:
  void Write(int32_t deviceAddress, const uint8_t* dataToSend,
             int32_t sendSize);
  void Read(int32_t deviceAddress, uint8_t* buffer, int32_t count);

  SimDataValue<HAL_Bool, HAL_MakeBoolean, GetInitializedName> initialized{
      false};
  SimCallbackRegistry<HAL_BufferCallback, GetReadName> read;
  SimCallbackRegistry<HAL_ConstBufferCallback, GetWriteName> write;

  void ResetData();
};
extern I2CData* SimI2CData;
}  // namespace hal

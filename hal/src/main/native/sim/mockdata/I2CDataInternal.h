/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "mockdata/I2CData.h"
#include "mockdata/SimCallbackRegistry.h"
#include "mockdata/SimDataValue.h"

namespace hal {
class I2CData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(Read)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(Write)

 public:
  void Write(int32_t deviceAddress, const uint8_t* dataToSend,
             int32_t sendSize);
  void Read(int32_t deviceAddress, uint8_t* buffer, int32_t count);

  SimDataValue<HAL_Bool, MakeBoolean, GetInitializedName> initialized{false};
  SimCallbackRegistry<HAL_BufferCallback, GetReadName> read;
  SimCallbackRegistry<HAL_ConstBufferCallback, GetWriteName> write;

  void ResetData();
};
extern I2CData* SimI2CData;
}  // namespace hal

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "mockdata/SPIData.h"
#include "mockdata/SimCallbackRegistry.h"
#include "mockdata/SimDataValue.h"

namespace hal {

class SPIData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Initialized)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(Read)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(Write)
  HAL_SIMCALLBACKREGISTRY_DEFINE_NAME(AutoReceive)

 public:
  int32_t Read(uint8_t* buffer, int32_t count);
  int32_t Write(const uint8_t* dataToSend, int32_t sendSize);
  int32_t Transaction(const uint8_t* dataToSend, uint8_t* dataReceived,
                      int32_t size);
  int32_t ReadAutoReceivedData(uint32_t* buffer, int32_t numToRead,
                               double timeout, int32_t* status);

  SimDataValue<HAL_Bool, MakeBoolean, GetInitializedName> initialized{false};
  SimCallbackRegistry<HAL_BufferCallback, GetReadName> read;
  SimCallbackRegistry<HAL_ConstBufferCallback, GetWriteName> write;
  SimCallbackRegistry<HAL_SpiReadAutoReceiveBufferCallback, GetAutoReceiveName>
      autoReceivedData;

  void ResetData();
};
extern SPIData* SimSPIData;
}  // namespace hal

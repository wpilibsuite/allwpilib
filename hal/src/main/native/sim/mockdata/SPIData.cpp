/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include "../PortsInternal.h"
#include "SPIDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeSPIData() {
  static SPIData ssd[5];
  ::hal::SimSPIData = ssd;
}
}  // namespace init
}  // namespace hal

SPIData* hal::SimSPIData;
void SPIData::ResetData() {
  initialized.Reset(false);
  read.Reset();
  write.Reset();
  autoReceivedData.Reset();
}

int32_t SPIData::Read(uint8_t* buffer, int32_t count) {
  read(buffer, count);
  return count;
}

int32_t SPIData::Write(const uint8_t* dataToSend, int32_t sendSize) {
  write(dataToSend, sendSize);
  return sendSize;
}

int32_t SPIData::Transaction(const uint8_t* dataToSend, uint8_t* dataReceived,
                             int32_t size) {
  write(dataToSend, size);
  read(dataReceived, size);
  return size;
}

int32_t SPIData::ReadAutoReceivedData(uint32_t* buffer, int32_t numToRead,
                                      double timeout, int32_t* status) {
  int32_t outputCount = 0;
  autoReceivedData(buffer, numToRead, &outputCount);
  return outputCount;
}

extern "C" {
void HALSIM_ResetSPIData(int32_t index) { SimSPIData[index].ResetData(); }

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                          \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, SPI##CAPINAME, SimSPIData, \
                               LOWERNAME)

DEFINE_CAPI(HAL_Bool, Initialized, initialized)

#undef DEFINE_CAPI
#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                                 \
  HAL_SIMCALLBACKREGISTRY_DEFINE_CAPI(TYPE, HALSIM, SPI##CAPINAME, SimSPIData, \
                                      LOWERNAME)

DEFINE_CAPI(HAL_BufferCallback, Read, read)
DEFINE_CAPI(HAL_ConstBufferCallback, Write, write)
DEFINE_CAPI(HAL_SpiReadAutoReceiveBufferCallback, ReadAutoReceivedData,
            autoReceivedData)

}  // extern "C"

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.h"
#include "I2CDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeI2CData() {
  static I2CData sid[kI2CPorts];
  ::hal::SimI2CData = sid;
}
}  // namespace hal::init

I2CData* hal::SimI2CData;

void I2CData::ResetData() {
  initialized.Reset(false);
  read.Reset();
  write.Reset();
}

void I2CData::Write(int32_t deviceAddress, const uint8_t* dataToSend,
                    int32_t sendSize) {
  write(dataToSend, sendSize);
}

void I2CData::Read(int32_t deviceAddress, uint8_t* buffer, int32_t count) {
  read(buffer, count);
}

extern "C" {
void HALSIM_ResetI2CData(int32_t index) {
  SimI2CData[index].ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                          \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, I2C##CAPINAME, SimI2CData, \
                               LOWERNAME)

DEFINE_CAPI(HAL_Bool, Initialized, initialized)

#undef DEFINE_CAPI
#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                                 \
  HAL_SIMCALLBACKREGISTRY_DEFINE_CAPI(TYPE, HALSIM, I2C##CAPINAME, SimI2CData, \
                                      LOWERNAME)

DEFINE_CAPI(HAL_BufferCallback, Read, read)
DEFINE_CAPI(HAL_ConstBufferCallback, Write, write)

}  // extern "C"

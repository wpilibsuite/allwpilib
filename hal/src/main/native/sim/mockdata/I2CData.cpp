/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <iostream>

#include "../PortsInternal.h"
#include "I2CDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeI2CData() {
  static I2CData sid[2];
  ::hal::SimI2CData = sid;
}
}  // namespace init
}  // namespace hal

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
void HALSIM_ResetI2CData(int32_t index) { SimI2CData[index].ResetData(); }

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

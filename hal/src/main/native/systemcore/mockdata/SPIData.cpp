// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/simulation/SPIData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetSPIData(int32_t index) {}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, SPI##CAPINAME, RETURN)

DEFINE_CAPI(HAL_Bool, Initialized, false)

#undef DEFINE_CAPI
#define DEFINE_CAPI(TYPE, CAPINAME) \
  HAL_SIMCALLBACKREGISTRY_STUB_CAPI(TYPE, HALSIM, SPI##CAPINAME)

DEFINE_CAPI(HAL_BufferCallback, Read)
DEFINE_CAPI(HAL_ConstBufferCallback, Write)
DEFINE_CAPI(HAL_SpiReadAutoReceiveBufferCallback, ReadAutoReceivedData)

}  // extern "C"

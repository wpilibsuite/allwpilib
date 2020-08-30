/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

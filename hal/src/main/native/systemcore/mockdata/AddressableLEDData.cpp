// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/simulation/AddressableLEDData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {

int32_t HALSIM_FindAddressableLEDForChannel(int32_t channel) {
  return 0;
}

void HALSIM_ResetAddressableLEDData(int32_t index) {}

int32_t HALSIM_GetAddressableLEDData(int32_t index,
                                     struct HAL_AddressableLEDData* data) {
  return 0;
}

void HALSIM_SetAddressableLEDData(int32_t index,
                                  const struct HAL_AddressableLEDData* data,
                                  int32_t length) {}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, AddressableLED##CAPINAME, RETURN)

DEFINE_CAPI(HAL_Bool, Initialized, false)
DEFINE_CAPI(int32_t, OutputPort, 0)
DEFINE_CAPI(int32_t, Length, 0)
DEFINE_CAPI(HAL_Bool, Running, false)

#undef DEFINE_CAPI
#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMCALLBACKREGISTRY_STUB_CAPI(TYPE, HALSIM, AddressableLED##CAPINAME)

DEFINE_CAPI(HAL_ConstBufferCallback, Data, data)

void HALSIM_RegisterAddressableLEDAllCallbacks(int32_t index,
                                               HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify) {}
}  // extern "C"

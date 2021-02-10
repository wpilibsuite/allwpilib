// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/simulation/RelayData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetRelayData(int32_t index) {}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, Relay##CAPINAME, RETURN)

DEFINE_CAPI(HAL_Bool, InitializedForward, false)
DEFINE_CAPI(HAL_Bool, InitializedReverse, false)
DEFINE_CAPI(HAL_Bool, Forward, false)
DEFINE_CAPI(HAL_Bool, Reverse, false)

void HALSIM_RegisterRelayAllCallbacks(int32_t index,
                                      HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify) {}
}  // extern "C"

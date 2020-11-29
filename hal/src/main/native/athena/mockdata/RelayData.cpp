/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/simulation/RelayData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetRelayData(int32_t index) {}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, Relay##CAPINAME, RETURN)

const char* HALSIM_GetRelayDisplayName(int32_t index) { return ""; }
void HALSIM_SetRelayDisplayName(int32_t index, const char* displayName) {}
DEFINE_CAPI(HAL_Bool, InitializedForward, false)
DEFINE_CAPI(HAL_Bool, InitializedReverse, false)
DEFINE_CAPI(HAL_Bool, Forward, false)
DEFINE_CAPI(HAL_Bool, Reverse, false)

void HALSIM_RegisterRelayAllCallbacks(int32_t index,
                                      HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify) {}
}  // extern "C"

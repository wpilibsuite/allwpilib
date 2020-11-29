/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/simulation/PCMData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetPCMData(int32_t index) {}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, PCM##CAPINAME, RETURN)

HAL_SIMDATAVALUE_STUB_CAPI_CHANNEL(HAL_Bool, HALSIM, PCMSolenoidInitialized,
                                   false)
HAL_SIMDATAVALUE_STUB_CAPI_CHANNEL(HAL_Bool, HALSIM, PCMSolenoidOutput, false)

const char* HALSIM_GetSolenoidDisplayName(int32_t index, int32_t channel) {
  return "";
}
void HALSIM_SetSolenoidDisplayName(int32_t index, int32_t channel,
                                   const char* displayName) {}
DEFINE_CAPI(HAL_Bool, CompressorInitialized, false)
DEFINE_CAPI(HAL_Bool, CompressorOn, false)
DEFINE_CAPI(HAL_Bool, ClosedLoopEnabled, false)
DEFINE_CAPI(HAL_Bool, PressureSwitch, false)
DEFINE_CAPI(double, CompressorCurrent, 0)

void HALSIM_GetPCMAllSolenoids(int32_t index, uint8_t* values) { *values = 0; }

void HALSIM_SetPCMAllSolenoids(int32_t index, uint8_t values) {}

void HALSIM_RegisterPCMAllNonSolenoidCallbacks(int32_t index,
                                               HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify) {}

void HALSIM_RegisterPCMAllSolenoidCallbacks(int32_t index, int32_t channel,
                                            HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify) {}
}  // extern "C"

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/simulation/CTREPCMData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetCTREPCMData(int32_t index) {}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, CTREPCM##CAPINAME, RETURN)

HAL_SIMDATAVALUE_STUB_CAPI_CHANNEL(HAL_Bool, HALSIM, CTREPCMSolenoidOutput,
                                   false)
DEFINE_CAPI(HAL_Bool, Initialized, false)
DEFINE_CAPI(HAL_Bool, CompressorOn, false)
DEFINE_CAPI(HAL_Bool, ClosedLoopEnabled, false)
DEFINE_CAPI(HAL_Bool, PressureSwitch, false)
DEFINE_CAPI(double, CompressorCurrent, 0)

void HALSIM_GetCTREPCMAllSolenoids(int32_t index, uint8_t* values) {
  *values = 0;
}

void HALSIM_SetCTREPCMAllSolenoids(int32_t index, uint8_t values) {}

void HALSIM_RegisterCTREPCMAllNonSolenoidCallbacks(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify) {}

void HALSIM_RegisterCTREPCMAllSolenoidCallbacks(int32_t index, int32_t channel,
                                                HAL_NotifyCallback callback,
                                                void* param,
                                                HAL_Bool initialNotify) {}
}  // extern "C"

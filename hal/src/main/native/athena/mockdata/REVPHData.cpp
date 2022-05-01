// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/simulation/REVPHData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetREVPHData(int32_t index) {}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, REVPH##CAPINAME, RETURN)

HAL_SIMDATAVALUE_STUB_CAPI_CHANNEL(HAL_Bool, HALSIM, REVPHSolenoidOutput, false)
DEFINE_CAPI(HAL_Bool, Initialized, false)
DEFINE_CAPI(HAL_Bool, CompressorOn, false)
DEFINE_CAPI(HAL_REVPHCompressorConfigType, CompressorConfigType,
            HAL_REVPHCompressorConfigType_kDisabled)
DEFINE_CAPI(HAL_Bool, PressureSwitch, false)
DEFINE_CAPI(double, CompressorCurrent, 0)

void HALSIM_GetREVPHAllSolenoids(int32_t index, uint8_t* values) {
  *values = 0;
}

void HALSIM_SetREVPHAllSolenoids(int32_t index, uint8_t values) {}

void HALSIM_RegisterREVPHAllNonSolenoidCallbacks(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify) {}

void HALSIM_RegisterREVPHAllSolenoidCallbacks(int32_t index, int32_t channel,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {}
}  // extern "C"

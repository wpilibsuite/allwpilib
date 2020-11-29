/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/simulation/DIOData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetDIOData(int32_t index) {}

HAL_SimDeviceHandle HALSIM_GetDIOSimDevice(int32_t index) { return 0; }

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, DIO##CAPINAME, RETURN)

const char* HALSIM_GetDIODisplayName(int32_t index) { return ""; }
void HALSIM_SetDIODisplayName(int32_t index, const char* displayName) {}
DEFINE_CAPI(HAL_Bool, Initialized, false)
DEFINE_CAPI(HAL_Bool, Value, false)
DEFINE_CAPI(double, PulseLength, 0)
DEFINE_CAPI(HAL_Bool, IsInput, false)
DEFINE_CAPI(int32_t, FilterIndex, 0)

void HALSIM_RegisterDIOAllCallbacks(int32_t index, HAL_NotifyCallback callback,
                                    void* param, HAL_Bool initialNotify) {}
}  // extern "C"

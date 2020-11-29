/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/simulation/PWMData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetPWMData(int32_t index) {}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, PWM##CAPINAME, RETURN)

const char* HALSIM_GetPWMDisplayName(int32_t index) { return ""; }
void HALSIM_SetPWMDisplayName(int32_t index, const char* displayName) {}
DEFINE_CAPI(HAL_Bool, Initialized, false)
DEFINE_CAPI(int32_t, RawValue, 0)
DEFINE_CAPI(double, Speed, 0)
DEFINE_CAPI(double, Position, 0)
DEFINE_CAPI(int32_t, PeriodScale, 0)
DEFINE_CAPI(HAL_Bool, ZeroLatch, false)

void HALSIM_RegisterPWMAllCallbacks(int32_t index, HAL_NotifyCallback callback,
                                    void* param, HAL_Bool initialNotify) {}
}  // extern "C"

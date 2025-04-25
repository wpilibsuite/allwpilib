// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/simulation/PWMData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetPWMData(int32_t index) {}

HAL_SimDeviceHandle HALSIM_GetPWMSimDevice(int32_t index) {
  return 0;
}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, PWM##CAPINAME, RETURN)

DEFINE_CAPI(HAL_Bool, Initialized, false)
DEFINE_CAPI(int32_t, PulseMicrosecond, 0)
DEFINE_CAPI(int32_t, OutputPeriod, 0)

void HALSIM_RegisterPWMAllCallbacks(int32_t index, HAL_NotifyCallback callback,
                                    void* param, HAL_Bool initialNotify) {}
}  // extern "C"

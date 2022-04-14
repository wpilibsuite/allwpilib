// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/simulation/DIOData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetDIOData(int32_t index) {}

HAL_SimDeviceHandle HALSIM_GetDIOSimDevice(int32_t index) {
  return 0;
}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, DIO##CAPINAME, RETURN)

DEFINE_CAPI(HAL_Bool, Initialized, false)
DEFINE_CAPI(HAL_Bool, Value, false)
DEFINE_CAPI(double, PulseLength, 0)
DEFINE_CAPI(HAL_Bool, IsInput, false)
DEFINE_CAPI(int32_t, FilterIndex, 0)

void HALSIM_RegisterDIOAllCallbacks(int32_t index, HAL_NotifyCallback callback,
                                    void* param, HAL_Bool initialNotify) {}
}  // extern "C"

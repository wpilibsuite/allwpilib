// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/simulation/AccelerometerData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetAccelerometerData(int32_t index) {}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, Accelerometer##CAPINAME, RETURN)

DEFINE_CAPI(HAL_Bool, Active, false)
DEFINE_CAPI(HAL_AccelerometerRange, Range, HAL_AccelerometerRange_k2G)
DEFINE_CAPI(double, X, 0)
DEFINE_CAPI(double, Y, 0)
DEFINE_CAPI(double, Z, 0)

void HALSIM_RegisterAccelerometerAllCallbacks(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {}
}  // extern "C"

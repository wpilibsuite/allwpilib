// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/simulation/AnalogGyroData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetAnalogGyroData(int32_t index) {}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, AnalogGyro##CAPINAME, RETURN)

DEFINE_CAPI(double, Angle, 0)
DEFINE_CAPI(double, Rate, 0)
DEFINE_CAPI(HAL_Bool, Initialized, false)

void HALSIM_RegisterAnalogGyroAllCallbacks(int32_t index,
                                           HAL_NotifyCallback callback,
                                           void* param,
                                           HAL_Bool initialNotify) {}
}  // extern "C"

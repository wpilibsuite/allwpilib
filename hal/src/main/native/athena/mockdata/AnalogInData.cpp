// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/simulation/AnalogInData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetAnalogInData(int32_t index) {}

HAL_SimDeviceHandle HALSIM_GetAnalogInSimDevice(int32_t index) {
  return 0;
}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, AnalogIn##CAPINAME, RETURN)

DEFINE_CAPI(HAL_Bool, Initialized, false)
DEFINE_CAPI(int32_t, AverageBits, 0)
DEFINE_CAPI(int32_t, OversampleBits, 0)
DEFINE_CAPI(double, Voltage, 0)
DEFINE_CAPI(HAL_Bool, AccumulatorInitialized, false)
DEFINE_CAPI(int64_t, AccumulatorValue, 0)
DEFINE_CAPI(int64_t, AccumulatorCount, 0)
DEFINE_CAPI(int32_t, AccumulatorCenter, 0)
DEFINE_CAPI(int32_t, AccumulatorDeadband, 0)

void HALSIM_RegisterAnalogInAllCallbacks(int32_t index,
                                         HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify) {}
}  // extern "C"

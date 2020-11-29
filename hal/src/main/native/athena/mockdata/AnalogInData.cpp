/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/simulation/AnalogInData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetAnalogInData(int32_t index) {}

HAL_SimDeviceHandle HALSIM_GetAnalogInSimDevice(int32_t index) { return 0; }

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, AnalogIn##CAPINAME, RETURN)

const char* HALSIM_GetAnalogInDisplayName(int32_t index) { return ""; }
void HALSIM_SetAnalogInDisplayName(int32_t index, const char* displayName) {}
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

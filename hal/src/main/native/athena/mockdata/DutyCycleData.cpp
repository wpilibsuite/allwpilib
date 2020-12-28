// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/simulation/DutyCycleData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
int32_t HALSIM_FindDutyCycleForChannel(int32_t channel) {
  return 0;
}

void HALSIM_ResetDutyCycleData(int32_t index) {}

int32_t HALSIM_GetDutyCycleDigitalChannel(int32_t index) {
  return 0;
}

HAL_SimDeviceHandle HALSIM_GetDutyCycleSimDevice(int32_t index) {
  return 0;
}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, DutyCycle##CAPINAME, RETURN)

DEFINE_CAPI(HAL_Bool, Initialized, false)
DEFINE_CAPI(int32_t, Frequency, 0)
DEFINE_CAPI(double, Output, 0)

void HALSIM_RegisterDutyCycleAllCallbacks(int32_t index,
                                          HAL_NotifyCallback callback,
                                          void* param, HAL_Bool initialNotify) {
}

}  // extern "C"

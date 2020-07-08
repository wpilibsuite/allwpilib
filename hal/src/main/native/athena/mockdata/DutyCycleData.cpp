/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/simulation/DutyCycleData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
int32_t HALSIM_FindDutyCycleForChannel(int32_t channel) { return 0; }

void HALSIM_ResetDutyCycleData(int32_t index) {}

int32_t HALSIM_GetDutyCycleDigitalChannel(int32_t index) { return 0; }

HAL_SimDeviceHandle HALSIM_GetDutyCycleSimDevice(int32_t index) { return 0; }

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

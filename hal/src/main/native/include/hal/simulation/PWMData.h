// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/Types.h"
#include "hal/simulation/NotifyListener.h"

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetPWMData(int32_t index);
int32_t HALSIM_RegisterPWMInitializedCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify);
void HALSIM_CancelPWMInitializedCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetPWMInitialized(int32_t index);
void HALSIM_SetPWMInitialized(int32_t index, HAL_Bool initialized);

HAL_SimDeviceHandle HALSIM_GetPWMSimDevice(int32_t index);

int32_t HALSIM_RegisterPWMPulseMicrosecondCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify);
void HALSIM_CancelPWMPulseMicrosecondCallback(int32_t index, int32_t uid);
int32_t HALSIM_GetPWMPulseMicrosecond(int32_t index);
void HALSIM_SetPWMPulseMicrosecond(int32_t index, int32_t microsecondPulseTime);

int32_t HALSIM_RegisterPWMOutputPeriodCallback(int32_t index,
                                               HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify);
void HALSIM_CancelPWMOutputPeriodCallback(int32_t index, int32_t uid);
int32_t HALSIM_GetPWMOutputPeriod(int32_t index);
void HALSIM_SetPWMOutputPeriod(int32_t index, int32_t periodScale);

void HALSIM_RegisterPWMAllCallbacks(int32_t index, HAL_NotifyCallback callback,
                                    void* param, HAL_Bool initialNotify);

#ifdef __cplusplus
}  // extern "C"
#endif

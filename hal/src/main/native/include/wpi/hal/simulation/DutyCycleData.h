// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/Types.h"
#include "wpi/hal/simulation/NotifyListener.h"

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetDutyCycleData(int32_t index);

int32_t HALSIM_RegisterDutyCycleInitializedCallback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify);
void HALSIM_CancelDutyCycleInitializedCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetDutyCycleInitialized(int32_t index);
void HALSIM_SetDutyCycleInitialized(int32_t index, HAL_Bool initialized);

HAL_SimDeviceHandle HALSIM_GetDutyCycleSimDevice(int32_t index);

int32_t HALSIM_RegisterDutyCycleOutputCallback(int32_t index,
                                               HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify);
void HALSIM_CancelDutyCycleOutputCallback(int32_t index, int32_t uid);
double HALSIM_GetDutyCycleOutput(int32_t index);
void HALSIM_SetDutyCycleOutput(int32_t index, double output);

int32_t HALSIM_RegisterDutyCycleFrequencyCallback(int32_t index,
                                                  HAL_NotifyCallback callback,
                                                  void* param,
                                                  HAL_Bool initialNotify);
void HALSIM_CancelDutyCycleFrequencyCallback(int32_t index, int32_t uid);
double HALSIM_GetDutyCycleFrequency(int32_t index);
void HALSIM_SetDutyCycleFrequency(int32_t index, double frequency);

void HALSIM_RegisterDutyCycleAllCallbacks(int32_t index,
                                          HAL_NotifyCallback callback,
                                          void* param, HAL_Bool initialNotify);

#ifdef __cplusplus
}  // extern "C"
#endif

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/Types.h"
#include "hal/simulation/NotifyListener.h"

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetAnalogInData(int32_t index);
int32_t HALSIM_RegisterAnalogInInitializedCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify);
void HALSIM_CancelAnalogInInitializedCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetAnalogInInitialized(int32_t index);
void HALSIM_SetAnalogInInitialized(int32_t index, HAL_Bool initialized);

HAL_SimDeviceHandle HALSIM_GetAnalogInSimDevice(int32_t index);

int32_t HALSIM_RegisterAnalogInAverageBitsCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify);
void HALSIM_CancelAnalogInAverageBitsCallback(int32_t index, int32_t uid);
int32_t HALSIM_GetAnalogInAverageBits(int32_t index);
void HALSIM_SetAnalogInAverageBits(int32_t index, int32_t averageBits);

int32_t HALSIM_RegisterAnalogInOversampleBitsCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelAnalogInOversampleBitsCallback(int32_t index, int32_t uid);
int32_t HALSIM_GetAnalogInOversampleBits(int32_t index);
void HALSIM_SetAnalogInOversampleBits(int32_t index, int32_t oversampleBits);

int32_t HALSIM_RegisterAnalogInVoltageCallback(int32_t index,
                                               HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify);
void HALSIM_CancelAnalogInVoltageCallback(int32_t index, int32_t uid);
double HALSIM_GetAnalogInVoltage(int32_t index);
void HALSIM_SetAnalogInVoltage(int32_t index, double voltage);

void HALSIM_RegisterAnalogInAllCallbacks(int32_t index,
                                         HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify);

#ifdef __cplusplus
}  // extern "C"
#endif

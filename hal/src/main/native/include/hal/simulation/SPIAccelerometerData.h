// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/Types.h"
#include "hal/simulation/NotifyListener.h"

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetSPIAccelerometerData(int32_t index);
int32_t HALSIM_RegisterSPIAccelerometerActiveCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelSPIAccelerometerActiveCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetSPIAccelerometerActive(int32_t index);
void HALSIM_SetSPIAccelerometerActive(int32_t index, HAL_Bool active);

int32_t HALSIM_RegisterSPIAccelerometerRangeCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelSPIAccelerometerRangeCallback(int32_t index, int32_t uid);
int32_t HALSIM_GetSPIAccelerometerRange(int32_t index);
void HALSIM_SetSPIAccelerometerRange(int32_t index, int32_t range);

int32_t HALSIM_RegisterSPIAccelerometerXCallback(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);
void HALSIM_CancelSPIAccelerometerXCallback(int32_t index, int32_t uid);
double HALSIM_GetSPIAccelerometerX(int32_t index);
void HALSIM_SetSPIAccelerometerX(int32_t index, double x);

int32_t HALSIM_RegisterSPIAccelerometerYCallback(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);
void HALSIM_CancelSPIAccelerometerYCallback(int32_t index, int32_t uid);
double HALSIM_GetSPIAccelerometerY(int32_t index);
void HALSIM_SetSPIAccelerometerY(int32_t index, double y);

int32_t HALSIM_RegisterSPIAccelerometerZCallback(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);
void HALSIM_CancelSPIAccelerometerZCallback(int32_t index, int32_t uid);
double HALSIM_GetSPIAccelerometerZ(int32_t index);
void HALSIM_SetSPIAccelerometerZ(int32_t index, double z);

void HALSIM_RegisterSPIAccelerometerAllCallbcaks(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);

#ifdef __cplusplus
}  // extern "C"
#endif

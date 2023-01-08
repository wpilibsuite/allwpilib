// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/REVPH.h"
#include "hal/Types.h"
#include "hal/simulation/NotifyListener.h"

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetREVPHData(int32_t index);
int32_t HALSIM_RegisterREVPHInitializedCallback(int32_t index,
                                                HAL_NotifyCallback callback,
                                                void* param,
                                                HAL_Bool initialNotify);
void HALSIM_CancelREVPHInitializedCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetREVPHInitialized(int32_t index);
void HALSIM_SetREVPHInitialized(int32_t index, HAL_Bool solenoidInitialized);

int32_t HALSIM_RegisterREVPHSolenoidOutputCallback(int32_t index,
                                                   int32_t channel,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify);
void HALSIM_CancelREVPHSolenoidOutputCallback(int32_t index, int32_t channel,
                                              int32_t uid);
HAL_Bool HALSIM_GetREVPHSolenoidOutput(int32_t index, int32_t channel);
void HALSIM_SetREVPHSolenoidOutput(int32_t index, int32_t channel,
                                   HAL_Bool solenoidOutput);

int32_t HALSIM_RegisterREVPHCompressorOnCallback(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);
void HALSIM_CancelREVPHCompressorOnCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetREVPHCompressorOn(int32_t index);
void HALSIM_SetREVPHCompressorOn(int32_t index, HAL_Bool compressorOn);

int32_t HALSIM_RegisterREVPHCompressorConfigTypeCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelREVPHCompressorConfigTypeCallback(int32_t index, int32_t uid);
HAL_REVPHCompressorConfigType HALSIM_GetREVPHCompressorConfigType(
    int32_t index);
void HALSIM_SetREVPHCompressorConfigType(
    int32_t index, HAL_REVPHCompressorConfigType configType);

int32_t HALSIM_RegisterREVPHPressureSwitchCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify);
void HALSIM_CancelREVPHPressureSwitchCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetREVPHPressureSwitch(int32_t index);
void HALSIM_SetREVPHPressureSwitch(int32_t index, HAL_Bool pressureSwitch);

int32_t HALSIM_RegisterREVPHCompressorCurrentCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelREVPHCompressorCurrentCallback(int32_t index, int32_t uid);
double HALSIM_GetREVPHCompressorCurrent(int32_t index);
void HALSIM_SetREVPHCompressorCurrent(int32_t index, double compressorCurrent);

void HALSIM_GetREVPHAllSolenoids(int32_t index, uint8_t* values);
void HALSIM_SetREVPHAllSolenoids(int32_t index, uint8_t values);

void HALSIM_RegisterREVPHAllNonSolenoidCallbacks(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);

void HALSIM_RegisterREVPHAllSolenoidCallbacks(int32_t index, int32_t channel,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify);

#ifdef __cplusplus
}  // extern "C"
#endif

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/Types.h"
#include "hal/simulation/NotifyListener.h"

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetCTREPCMData(int32_t index);
int32_t HALSIM_RegisterCTREPCMInitializedCallback(int32_t index,
                                                  HAL_NotifyCallback callback,
                                                  void* param,
                                                  HAL_Bool initialNotify);
void HALSIM_CancelCTREPCMInitializedCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetCTREPCMInitialized(int32_t index);
void HALSIM_SetCTREPCMInitialized(int32_t index, HAL_Bool solenoidInitialized);

int32_t HALSIM_RegisterCTREPCMSolenoidOutputCallback(
    int32_t index, int32_t channel, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelCTREPCMSolenoidOutputCallback(int32_t index, int32_t channel,
                                                int32_t uid);
HAL_Bool HALSIM_GetCTREPCMSolenoidOutput(int32_t index, int32_t channel);
void HALSIM_SetCTREPCMSolenoidOutput(int32_t index, int32_t channel,
                                     HAL_Bool solenoidOutput);

int32_t HALSIM_RegisterCTREPCMCompressorOnCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify);
void HALSIM_CancelCTREPCMCompressorOnCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetCTREPCMCompressorOn(int32_t index);
void HALSIM_SetCTREPCMCompressorOn(int32_t index, HAL_Bool compressorOn);

int32_t HALSIM_RegisterCTREPCMClosedLoopEnabledCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelCTREPCMClosedLoopEnabledCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetCTREPCMClosedLoopEnabled(int32_t index);
void HALSIM_SetCTREPCMClosedLoopEnabled(int32_t index,
                                        HAL_Bool closedLoopEnabled);

int32_t HALSIM_RegisterCTREPCMPressureSwitchCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelCTREPCMPressureSwitchCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetCTREPCMPressureSwitch(int32_t index);
void HALSIM_SetCTREPCMPressureSwitch(int32_t index, HAL_Bool pressureSwitch);

int32_t HALSIM_RegisterCTREPCMCompressorCurrentCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelCTREPCMCompressorCurrentCallback(int32_t index, int32_t uid);
double HALSIM_GetCTREPCMCompressorCurrent(int32_t index);
void HALSIM_SetCTREPCMCompressorCurrent(int32_t index,
                                        double compressorCurrent);

void HALSIM_GetCTREPCMAllSolenoids(int32_t index, uint8_t* values);
void HALSIM_SetCTREPCMAllSolenoids(int32_t index, uint8_t values);

void HALSIM_RegisterCTREPCMAllNonSolenoidCallbacks(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify);

void HALSIM_RegisterCTREPCMAllSolenoidCallbacks(int32_t index, int32_t channel,
                                                HAL_NotifyCallback callback,
                                                void* param,
                                                HAL_Bool initialNotify);

#ifdef __cplusplus
}  // extern "C"
#endif

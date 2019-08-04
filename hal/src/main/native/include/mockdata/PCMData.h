/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#ifndef __FRC_ROBORIO__

#include "NotifyListener.h"
#include "hal/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetPCMData(int32_t index);
int32_t HALSIM_RegisterPCMSolenoidInitializedCallback(
    int32_t index, int32_t channel, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelPCMSolenoidInitializedCallback(int32_t index, int32_t channel,
                                                 int32_t uid);
HAL_Bool HALSIM_GetPCMSolenoidInitialized(int32_t index, int32_t channel);
void HALSIM_SetPCMSolenoidInitialized(int32_t index, int32_t channel,
                                      HAL_Bool solenoidInitialized);

int32_t HALSIM_RegisterPCMSolenoidOutputCallback(int32_t index, int32_t channel,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);
void HALSIM_CancelPCMSolenoidOutputCallback(int32_t index, int32_t channel,
                                            int32_t uid);
HAL_Bool HALSIM_GetPCMSolenoidOutput(int32_t index, int32_t channel);
void HALSIM_SetPCMSolenoidOutput(int32_t index, int32_t channel,
                                 HAL_Bool solenoidOutput);

int32_t HALSIM_RegisterPCMCompressorInitializedCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelPCMCompressorInitializedCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetPCMCompressorInitialized(int32_t index);
void HALSIM_SetPCMCompressorInitialized(int32_t index,
                                        HAL_Bool compressorInitialized);

int32_t HALSIM_RegisterPCMCompressorOnCallback(int32_t index,
                                               HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify);
void HALSIM_CancelPCMCompressorOnCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetPCMCompressorOn(int32_t index);
void HALSIM_SetPCMCompressorOn(int32_t index, HAL_Bool compressorOn);

int32_t HALSIM_RegisterPCMClosedLoopEnabledCallback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify);
void HALSIM_CancelPCMClosedLoopEnabledCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetPCMClosedLoopEnabled(int32_t index);
void HALSIM_SetPCMClosedLoopEnabled(int32_t index, HAL_Bool closedLoopEnabled);

int32_t HALSIM_RegisterPCMPressureSwitchCallback(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);
void HALSIM_CancelPCMPressureSwitchCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetPCMPressureSwitch(int32_t index);
void HALSIM_SetPCMPressureSwitch(int32_t index, HAL_Bool pressureSwitch);

int32_t HALSIM_RegisterPCMCompressorCurrentCallback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify);
void HALSIM_CancelPCMCompressorCurrentCallback(int32_t index, int32_t uid);
double HALSIM_GetPCMCompressorCurrent(int32_t index);
void HALSIM_SetPCMCompressorCurrent(int32_t index, double compressorCurrent);

void HALSIM_RegisterPCMAllNonSolenoidCallbacks(int32_t index,
                                               HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify);

void HALSIM_RegisterPCMAllSolenoidCallbacks(int32_t index, int32_t channel,
                                            HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "hal/Types.h"
#include "hal/simulation/NotifyListener.h"

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetPDPData(int32_t index);

const char* HALSIM_GetPDPDisplayName(int32_t index);
void HALSIM_SetPDPDisplayName(int32_t index, const char* displayName);

int32_t HALSIM_RegisterPDPInitializedCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify);
void HALSIM_CancelPDPInitializedCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetPDPInitialized(int32_t index);
void HALSIM_SetPDPInitialized(int32_t index, HAL_Bool initialized);

int32_t HALSIM_RegisterPDPTemperatureCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify);
void HALSIM_CancelPDPTemperatureCallback(int32_t index, int32_t uid);
double HALSIM_GetPDPTemperature(int32_t index);
void HALSIM_SetPDPTemperature(int32_t index, double temperature);

int32_t HALSIM_RegisterPDPVoltageCallback(int32_t index,
                                          HAL_NotifyCallback callback,
                                          void* param, HAL_Bool initialNotify);
void HALSIM_CancelPDPVoltageCallback(int32_t index, int32_t uid);
double HALSIM_GetPDPVoltage(int32_t index);
void HALSIM_SetPDPVoltage(int32_t index, double voltage);

int32_t HALSIM_RegisterPDPCurrentCallback(int32_t index, int32_t channel,
                                          HAL_NotifyCallback callback,
                                          void* param, HAL_Bool initialNotify);
void HALSIM_CancelPDPCurrentCallback(int32_t index, int32_t channel,
                                     int32_t uid);
double HALSIM_GetPDPCurrent(int32_t index, int32_t channel);
void HALSIM_SetPDPCurrent(int32_t index, int32_t channel, double current);

void HALSIM_GetPDPAllCurrents(int32_t index, double* currents);
void HALSIM_SetPDPAllCurrents(int32_t index, const double* currents);

void HALSIM_RegisterPDPAllNonCurrentCallbacks(int32_t index, int32_t channel,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify);

#ifdef __cplusplus
}  // extern "C"
#endif

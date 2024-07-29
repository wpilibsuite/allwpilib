// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/Types.h"
#include "hal/simulation/NotifyListener.h"

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetPowerDistributionData(int32_t index);
int32_t HALSIM_RegisterPowerDistributionInitializedCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelPowerDistributionInitializedCallback(int32_t index,
                                                       int32_t uid);
HAL_Bool HALSIM_GetPowerDistributionInitialized(int32_t index);
void HALSIM_SetPowerDistributionInitialized(int32_t index,
                                            HAL_Bool initialized);

int32_t HALSIM_RegisterPowerDistributionTemperatureCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelPowerDistributionTemperatureCallback(int32_t index,
                                                       int32_t uid);
double HALSIM_GetPowerDistributionTemperature(int32_t index);
void HALSIM_SetPowerDistributionTemperature(int32_t index, double temperature);

int32_t HALSIM_RegisterPowerDistributionVoltageCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelPowerDistributionVoltageCallback(int32_t index, int32_t uid);
double HALSIM_GetPowerDistributionVoltage(int32_t index);
void HALSIM_SetPowerDistributionVoltage(int32_t index, double voltage);

int32_t HALSIM_RegisterPowerDistributionCurrentCallback(
    int32_t index, int32_t channel, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelPowerDistributionCurrentCallback(int32_t index,
                                                   int32_t channel,
                                                   int32_t uid);
double HALSIM_GetPowerDistributionCurrent(int32_t index, int32_t channel);
void HALSIM_SetPowerDistributionCurrent(int32_t index, int32_t channel,
                                        double current);

void HALSIM_GetPowerDistributionAllCurrents(int32_t index, double* currents,
                                            int length);
void HALSIM_SetPowerDistributionAllCurrents(int32_t index,
                                            const double* currents, int length);

void HALSIM_RegisterPowerDistributionAllNonCurrentCallbacks(
    int32_t index, int32_t channel, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);

#ifdef __cplusplus
}  // extern "C"
#endif

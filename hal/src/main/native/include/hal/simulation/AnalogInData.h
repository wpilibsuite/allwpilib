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

void HALSIM_ResetAnalogInData(int32_t index);

const char* HALSIM_GetAnalogInDisplayName(int32_t index);
void HALSIM_SetAnalogInDisplayName(int32_t index, const char* displayName);

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

int32_t HALSIM_RegisterAnalogInAccumulatorInitializedCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelAnalogInAccumulatorInitializedCallback(int32_t index,
                                                         int32_t uid);
HAL_Bool HALSIM_GetAnalogInAccumulatorInitialized(int32_t index);
void HALSIM_SetAnalogInAccumulatorInitialized(int32_t index,
                                              HAL_Bool accumulatorInitialized);

int32_t HALSIM_RegisterAnalogInAccumulatorValueCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelAnalogInAccumulatorValueCallback(int32_t index, int32_t uid);
int64_t HALSIM_GetAnalogInAccumulatorValue(int32_t index);
void HALSIM_SetAnalogInAccumulatorValue(int32_t index,
                                        int64_t accumulatorValue);

int32_t HALSIM_RegisterAnalogInAccumulatorCountCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelAnalogInAccumulatorCountCallback(int32_t index, int32_t uid);
int64_t HALSIM_GetAnalogInAccumulatorCount(int32_t index);
void HALSIM_SetAnalogInAccumulatorCount(int32_t index,
                                        int64_t accumulatorCount);

int32_t HALSIM_RegisterAnalogInAccumulatorCenterCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelAnalogInAccumulatorCenterCallback(int32_t index, int32_t uid);
int32_t HALSIM_GetAnalogInAccumulatorCenter(int32_t index);
void HALSIM_SetAnalogInAccumulatorCenter(int32_t index,
                                         int32_t accumulatorCenter);

int32_t HALSIM_RegisterAnalogInAccumulatorDeadbandCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelAnalogInAccumulatorDeadbandCallback(int32_t index,
                                                      int32_t uid);
int32_t HALSIM_GetAnalogInAccumulatorDeadband(int32_t index);
void HALSIM_SetAnalogInAccumulatorDeadband(int32_t index,
                                           int32_t accumulatorDeadband);

void HALSIM_RegisterAnalogInAllCallbacks(int32_t index,
                                         HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify);

#ifdef __cplusplus
}  // extern "C"
#endif

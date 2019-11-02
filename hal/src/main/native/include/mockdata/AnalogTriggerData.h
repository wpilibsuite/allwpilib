/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "NotifyListener.h"
#include "hal/Types.h"

enum HALSIM_AnalogTriggerMode : int32_t {
  HALSIM_AnalogTriggerUnassigned,
  HALSIM_AnalogTriggerFiltered,
  HALSIM_AnalogTriggerDutyCycle,
  HALSIM_AnalogTriggerAveraged
};

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetAnalogTriggerData(int32_t index);
int32_t HALSIM_RegisterAnalogTriggerInitializedCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelAnalogTriggerInitializedCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetAnalogTriggerInitialized(int32_t index);
void HALSIM_SetAnalogTriggerInitialized(int32_t index, HAL_Bool initialized);

int32_t HALSIM_RegisterAnalogTriggerTriggerLowerBoundCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelAnalogTriggerTriggerLowerBoundCallback(int32_t index,
                                                         int32_t uid);
double HALSIM_GetAnalogTriggerTriggerLowerBound(int32_t index);
void HALSIM_SetAnalogTriggerTriggerLowerBound(int32_t index,
                                              double triggerLowerBound);

int32_t HALSIM_RegisterAnalogTriggerTriggerUpperBoundCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelAnalogTriggerTriggerUpperBoundCallback(int32_t index,
                                                         int32_t uid);
double HALSIM_GetAnalogTriggerTriggerUpperBound(int32_t index);
void HALSIM_SetAnalogTriggerTriggerUpperBound(int32_t index,
                                              double triggerUpperBound);

int32_t HALSIM_RegisterAnalogTriggerTriggerModeCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelAnalogTriggerTriggerModeCallback(int32_t index, int32_t uid);
HALSIM_AnalogTriggerMode HALSIM_GetAnalogTriggerTriggerMode(int32_t index);
void HALSIM_SetAnalogTriggerTriggerMode(int32_t index,
                                        HALSIM_AnalogTriggerMode triggerMode);

void HALSIM_RegisterAnalogTriggerAllCallbacks(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify);

#ifdef __cplusplus
}  // extern "C"
#endif

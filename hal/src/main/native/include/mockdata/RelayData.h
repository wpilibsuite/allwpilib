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

void HALSIM_ResetRelayData(int32_t index);
int32_t HALSIM_RegisterRelayInitializedForwardCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelRelayInitializedForwardCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetRelayInitializedForward(int32_t index);
void HALSIM_SetRelayInitializedForward(int32_t index,
                                       HAL_Bool initializedForward);

int32_t HALSIM_RegisterRelayInitializedReverseCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelRelayInitializedReverseCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetRelayInitializedReverse(int32_t index);
void HALSIM_SetRelayInitializedReverse(int32_t index,
                                       HAL_Bool initializedReverse);

int32_t HALSIM_RegisterRelayForwardCallback(int32_t index,
                                            HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify);
void HALSIM_CancelRelayForwardCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetRelayForward(int32_t index);
void HALSIM_SetRelayForward(int32_t index, HAL_Bool forward);

int32_t HALSIM_RegisterRelayReverseCallback(int32_t index,
                                            HAL_NotifyCallback callback,
                                            void* param,
                                            HAL_Bool initialNotify);
void HALSIM_CancelRelayReverseCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetRelayReverse(int32_t index);
void HALSIM_SetRelayReverse(int32_t index, HAL_Bool reverse);

void HALSIM_RegisterRelayAllCallbacks(int32_t index,
                                      HAL_NotifyCallback callback, void* param,
                                      HAL_Bool initialNotify);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif

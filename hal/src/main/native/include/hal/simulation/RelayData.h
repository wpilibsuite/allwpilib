// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/Types.h"
#include "hal/simulation/NotifyListener.h"

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

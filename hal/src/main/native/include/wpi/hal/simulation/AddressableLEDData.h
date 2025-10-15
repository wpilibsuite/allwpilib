// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/AddressableLEDTypes.h"
#include "hal/Types.h"
#include "hal/simulation/NotifyListener.h"

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetAddressableLEDData(int32_t index);

int32_t HALSIM_RegisterAddressableLEDInitializedCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelAddressableLEDInitializedCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetAddressableLEDInitialized(int32_t index);
void HALSIM_SetAddressableLEDInitialized(int32_t index, HAL_Bool initialized);

int32_t HALSIM_RegisterAddressableLEDStartCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify);
void HALSIM_CancelAddressableLEDStartCallback(int32_t index, int32_t uid);
int32_t HALSIM_GetAddressableLEDStart(int32_t index);
void HALSIM_SetAddressableLEDStart(int32_t index, int32_t start);

int32_t HALSIM_RegisterAddressableLEDLengthCallback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify);
void HALSIM_CancelAddressableLEDLengthCallback(int32_t index, int32_t uid);
int32_t HALSIM_GetAddressableLEDLength(int32_t index);
void HALSIM_SetAddressableLEDLength(int32_t index, int32_t length);

int32_t HALSIM_RegisterAddressableLEDDataCallback(
    HAL_ConstBufferCallback callback, void* param);
void HALSIM_CancelAddressableLEDDataCallback(int32_t uid);
int32_t HALSIM_GetAddressableLEDData(int32_t start, int32_t length,
                                     struct HAL_AddressableLEDData* data);
void HALSIM_SetAddressableLEDData(int32_t start, int32_t length,
                                  const struct HAL_AddressableLEDData* data);

void HALSIM_RegisterAddressableLEDAllCallbacks(int32_t index,
                                               HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify);

#ifdef __cplusplus
}  // extern "C"
#endif

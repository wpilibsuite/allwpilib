/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "NotifyListener.h"
#include "hal/AddressableLEDTypes.h"
#include "hal/Types.h"

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

int32_t HALSIM_RegisterAddressableLEDOutputPortCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelAddressableLEDOutputPortCallback(int32_t index, int32_t uid);
int32_t HALSIM_GetAddressableLEDOutputPort(int32_t index);
void HALSIM_SetAddressableLEDOutputPort(int32_t index, int32_t outputPort);

int32_t HALSIM_RegisterAddressableLEDLengthCallback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify);
void HALSIM_CancelAddressableLEDLengthCallback(int32_t index, int32_t uid);
int32_t HALSIM_GetAddressableLEDLength(int32_t index);
void HALSIM_SetAddressableLEDLength(int32_t index, int32_t length);

int32_t HALSIM_RegisterAddressableLEDRunningCallback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelAddressableLEDRunningCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetAddressableLEDRunning(int32_t index);
void HALSIM_SetAddressableLEDRunning(int32_t index, HAL_Bool running);

int32_t HALSIM_RegisterAddressableLEDDataCallback(
    int32_t index, HAL_ConstBufferCallback callback, void* param);
void HALSIM_CancelAddressableLEDDataCallback(int32_t index, int32_t uid);
int32_t HALSIM_GetAddressableLEDData(int32_t index,
                                     struct HAL_AddressableLEDData* data);
void HALSIM_SetAddressableLEDData(int32_t index,
                                  const struct HAL_AddressableLEDData* data,
                                  int32_t length);

void HALSIM_RegisterAddressableLEDAllCallbacks(int32_t index,
                                               HAL_NotifyCallback callback,
                                               void* param,
                                               HAL_Bool initialNotify);

#ifdef __cplusplus
}  // extern "C"
#endif

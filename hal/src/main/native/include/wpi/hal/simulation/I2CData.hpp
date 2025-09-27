// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/Types.h"
#include "hal/simulation/NotifyListener.h"

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetI2CData(int32_t index);

int32_t HALSIM_RegisterI2CInitializedCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify);
void HALSIM_CancelI2CInitializedCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetI2CInitialized(int32_t index);
void HALSIM_SetI2CInitialized(int32_t index, HAL_Bool initialized);

int32_t HALSIM_RegisterI2CReadCallback(int32_t index,
                                       HAL_BufferCallback callback,
                                       void* param);
void HALSIM_CancelI2CReadCallback(int32_t index, int32_t uid);

int32_t HALSIM_RegisterI2CWriteCallback(int32_t index,
                                        HAL_ConstBufferCallback callback,
                                        void* param);
void HALSIM_CancelI2CWriteCallback(int32_t index, int32_t uid);

#ifdef __cplusplus
}  // extern "C"
#endif

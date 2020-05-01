/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "NotifyListener.h"
#include "hal/Types.h"

typedef void (*HAL_SpiReadAutoReceiveBufferCallback)(const char* name,
                                                     void* param,
                                                     uint32_t* buffer,
                                                     int32_t numToRead,
                                                     int32_t* outputCount);

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetSPIData(int32_t index);

int32_t HALSIM_RegisterSPIInitializedCallback(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify);
void HALSIM_CancelSPIInitializedCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetSPIInitialized(int32_t index);
void HALSIM_SetSPIInitialized(int32_t index, HAL_Bool initialized);

int32_t HALSIM_RegisterSPIReadCallback(int32_t index,
                                       HAL_BufferCallback callback,
                                       void* param);
void HALSIM_CancelSPIReadCallback(int32_t index, int32_t uid);

int32_t HALSIM_RegisterSPIWriteCallback(int32_t index,
                                        HAL_ConstBufferCallback callback,
                                        void* param);
void HALSIM_CancelSPIWriteCallback(int32_t index, int32_t uid);

int32_t HALSIM_RegisterSPIReadAutoReceivedDataCallback(
    int32_t index, HAL_SpiReadAutoReceiveBufferCallback callback, void* param);
void HALSIM_CancelSPIReadAutoReceivedDataCallback(int32_t index, int32_t uid);

#ifdef __cplusplus
}  // extern "C"
#endif

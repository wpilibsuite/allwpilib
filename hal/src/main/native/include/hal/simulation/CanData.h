// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/Types.h"
#include "hal/Value.h"
#include "hal/simulation/NotifyListener.h"

typedef void (*HAL_CAN_SendMessageCallback)(const char* name, void* param,
                                            uint32_t messageID,
                                            const uint8_t* data,
                                            uint8_t dataSize, int32_t periodMs,
                                            int32_t* status);

typedef void (*HAL_CAN_ReceiveMessageCallback)(
    const char* name, void* param, uint32_t* messageID, uint32_t messageIDMask,
    uint8_t* data, uint8_t* dataSize, uint32_t* timeStamp, int32_t* status);

typedef void (*HAL_CAN_OpenStreamSessionCallback)(
    const char* name, void* param, uint32_t* sessionHandle, uint32_t messageID,
    uint32_t messageIDMask, uint32_t maxMessages, int32_t* status);

typedef void (*HAL_CAN_CloseStreamSessionCallback)(const char* name,
                                                   void* param,
                                                   uint32_t sessionHandle);

typedef void (*HAL_CAN_ReadStreamSessionCallback)(
    const char* name, void* param, uint32_t sessionHandle,
    struct HAL_CANStreamMessage* messages, uint32_t messagesToRead,
    uint32_t* messagesRead, int32_t* status);

typedef void (*HAL_CAN_GetCANStatusCallback)(
    const char* name, void* param, float* percentBusUtilization,
    uint32_t* busOffCount, uint32_t* txFullCount, uint32_t* receiveErrorCount,
    uint32_t* transmitErrorCount, int32_t* status);

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetCanData(void);

int32_t HALSIM_RegisterCanSendMessageCallback(
    HAL_CAN_SendMessageCallback callback, void* param);
void HALSIM_CancelCanSendMessageCallback(int32_t uid);

int32_t HALSIM_RegisterCanReceiveMessageCallback(
    HAL_CAN_ReceiveMessageCallback callback, void* param);
void HALSIM_CancelCanReceiveMessageCallback(int32_t uid);

int32_t HALSIM_RegisterCanOpenStreamCallback(
    HAL_CAN_OpenStreamSessionCallback callback, void* param);
void HALSIM_CancelCanOpenStreamCallback(int32_t uid);

int32_t HALSIM_RegisterCanCloseStreamCallback(
    HAL_CAN_CloseStreamSessionCallback callback, void* param);
void HALSIM_CancelCanCloseStreamCallback(int32_t uid);

int32_t HALSIM_RegisterCanReadStreamCallback(
    HAL_CAN_ReadStreamSessionCallback callback, void* param);
void HALSIM_CancelCanReadStreamCallback(int32_t uid);

int32_t HALSIM_RegisterCanGetCANStatusCallback(
    HAL_CAN_GetCANStatusCallback callback, void* param);
void HALSIM_CancelCanGetCANStatusCallback(int32_t uid);

#ifdef __cplusplus
}  // extern "C"
#endif

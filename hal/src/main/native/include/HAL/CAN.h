/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/Types.h"

// These are copies of defines located in CANSessionMux.h prepended with HAL_

#define HAL_CAN_SEND_PERIOD_NO_REPEAT 0
#define HAL_CAN_SEND_PERIOD_STOP_REPEATING -1

/* Flags in the upper bits of the messageID */
#define HAL_CAN_IS_FRAME_REMOTE 0x80000000
#define HAL_CAN_IS_FRAME_11BIT 0x40000000

#define HAL_ERR_CANSessionMux_InvalidBuffer -44086
#define HAL_ERR_CANSessionMux_MessageNotFound -44087
#define HAL_WARN_CANSessionMux_NoToken 44087
#define HAL_ERR_CANSessionMux_NotAllowed -44088
#define HAL_ERR_CANSessionMux_NotInitialized -44089
#define HAL_ERR_CANSessionMux_SessionOverrun 44050

struct HAL_CANStreamMessage {
  uint32_t messageID;
  uint32_t timeStamp;
  uint8_t data[8];
  uint8_t dataSize;
};

#ifdef __cplusplus
extern "C" {
#endif

void HAL_CAN_SendMessage(uint32_t messageID, const uint8_t* data,
                         uint8_t dataSize, int32_t periodMs, int32_t* status);
void HAL_CAN_ReceiveMessage(uint32_t* messageID, uint32_t messageIDMask,
                            uint8_t* data, uint8_t* dataSize,
                            uint32_t* timeStamp, int32_t* status);
void HAL_CAN_OpenStreamSession(uint32_t* sessionHandle, uint32_t messageID,
                               uint32_t messageIDMask, uint32_t maxMessages,
                               int32_t* status);
void HAL_CAN_CloseStreamSession(uint32_t sessionHandle);
void HAL_CAN_ReadStreamSession(uint32_t sessionHandle,
                               struct HAL_CANStreamMessage* messages,
                               uint32_t messagesToRead, uint32_t* messagesRead,
                               int32_t* status);
void HAL_CAN_GetCANStatus(float* percentBusUtilization, uint32_t* busOffCount,
                          uint32_t* txFullCount, uint32_t* receiveErrorCount,
                          uint32_t* transmitErrorCount, int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif

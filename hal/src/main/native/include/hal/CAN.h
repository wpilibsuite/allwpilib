// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

/**
 * @defgroup hal_can CAN Functions
 * @ingroup hal_capi
 * @{
 */
// These are copies of defines located in CANSessionMux.h prepended with HAL_

/**
 * Flag for sending a CAN message once.
 */
#define HAL_CAN_SEND_PERIOD_NO_REPEAT 0

/**
 * Flag for stopping periodic CAN message sends.
 */
#define HAL_CAN_SEND_PERIOD_STOP_REPEATING -1

/**
 * Mask for "is frame remote" in message ID.
 */
#define HAL_CAN_IS_FRAME_REMOTE 0x80000000

/**
 * Mask for "is frame 11 bits" in message ID.
 */
#define HAL_CAN_IS_FRAME_11BIT 0x40000000

#define HAL_ERR_CANSessionMux_InvalidBuffer -44086
#define HAL_ERR_CANSessionMux_MessageNotFound -44087
#define HAL_WARN_CANSessionMux_NoToken 44087
#define HAL_ERR_CANSessionMux_NotAllowed -44088
#define HAL_ERR_CANSessionMux_NotInitialized -44089
#define HAL_ERR_CANSessionMux_SessionOverrun 44050
/** @} */

/**
 * @defgroup hal_canstream CAN Stream Functions
 * @ingroup hal_capi
 * @{
 */
/**
 * Storage for CAN Stream Messages.
 */
struct HAL_CANStreamMessage {
  /** The message ID */
  uint32_t messageID;
  /** The packet received timestamp (based off of CLOCK_MONOTONIC) */
  uint32_t timeStamp;
  /** The message data */
  uint8_t data[8];
  /** The size of the data received (0-8 bytes) */
  uint8_t dataSize;
};
/** @} */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @ingroup hal_can
 * @{
 */
/**
 * Sends a CAN message.
 *
 * @param[in] messageID the CAN ID to send
 * @param[in] data      the data to send (0-8 bytes)
 * @param[in] dataSize  the size of the data to send (0-8 bytes)
 * @param[in] periodMs  the period to repeat the packet at. Use
 *                       HAL_CAN_SEND_PERIOD_NO_REPEAT to not repeat.
 * @param[out] status    Error status variable. 0 on success.
 */
void HAL_CAN_SendMessage(uint32_t messageID, const uint8_t* data,
                         uint8_t dataSize, int32_t periodMs, int32_t* status);

/**
 * Receives a CAN message.
 *
 * @param[out] messageID     store for the received message ID
 * @param[in] messageIDMask the message ID mask to look for
 * @param[out] data          data output (8 bytes)
 * @param[out] dataSize      data length (0-8 bytes)
 * @param[out] timeStamp     the packet received timestamp (based off of
 *                           CLOCK_MONOTONIC)
 * @param[out] status        Error status variable. 0 on success.
 */
void HAL_CAN_ReceiveMessage(uint32_t* messageID, uint32_t messageIDMask,
                            uint8_t* data, uint8_t* dataSize,
                            uint32_t* timeStamp, int32_t* status);
/**
 * Gets CAN status information.
 *
 * @param[out] percentBusUtilization the bus utilization
 * @param[out] busOffCount           the number of bus off errors
 * @param[out] txFullCount           the number of tx full errors
 * @param[out] receiveErrorCount     the number of receive errors
 * @param[out] transmitErrorCount    the number of transmit errors
 * @param[out] status                Error status variable. 0 on success.
 */
void HAL_CAN_GetCANStatus(float* percentBusUtilization, uint32_t* busOffCount,
                          uint32_t* txFullCount, uint32_t* receiveErrorCount,
                          uint32_t* transmitErrorCount, int32_t* status);
/** @} */
/**
 * @ingroup hal_canstream
 * @{
 */
/**
 * Opens a CAN stream.
 *
 * @param[out] sessionHandle output for the session handle
 * @param[in] messageID     the message ID to read
 * @param[in] messageIDMask the message ID mask
 * @param[in] maxMessages   the maximum number of messages to stream
 * @param[out] status        Error status variable. 0 on success.
 */
void HAL_CAN_OpenStreamSession(uint32_t* sessionHandle, uint32_t messageID,
                               uint32_t messageIDMask, uint32_t maxMessages,
                               int32_t* status);

/**
 * Closes a CAN stream.
 *
 * @param sessionHandle the session to close
 */
void HAL_CAN_CloseStreamSession(uint32_t sessionHandle);

/**
 * Reads a CAN stream message.
 *
 * @param[in] sessionHandle  the session handle
 * @param[in] messages       array of messages
 * @param[in] messagesToRead the max number of messages to read
 * @param[out] messagesRead   the number of messages actually read
 * @param[out] status         Error status variable. 0 on success.
 */
void HAL_CAN_ReadStreamSession(uint32_t sessionHandle,
                               struct HAL_CANStreamMessage* messages,
                               uint32_t messagesToRead, uint32_t* messagesRead,
                               int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */

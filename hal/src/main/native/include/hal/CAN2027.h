// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/CAN2027APITypes.h>
#include <hal/Types.h>
#include <stdint.h>

typedef HAL_Handle HAL_CAN2027StreamHandle;

/** @} */

/**
 * @defgroup hal_canstream CAN Stream Functions
 * @ingroup hal_capi
 * @{
 */

/**
 * Storage for CAN Stream Messages.
 */
struct HAL_CAN2027StreamMessage {
  /** The message ID */
  uint32_t messageId;
  /** The message */
  struct HAL_CAN2027ReceiveMessage message;
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
 * @param[in] busId     the CAN bus number
 * @param[in] messageId the message id
 * @param[in] message   the CAN message
 * @param[in] periodMs  the repeat period
 * @param[out] status   Error status variable. 0 on success.
 */
void HAL_CAN2027_SendMessage(int32_t busId, uint32_t messageId,
                         const struct HAL_CAN2027Message* message, int32_t periodMs,
                         int32_t* status);

/**
 * Receives a CAN message.
 *
 * @param[in] busId       The CAN bus number
 * @param[in] messageId the message id
 * @param[out] message  The CAN message
 * @param[out] status     Error status variable. 0 on success.
 */
void HAL_CAN2027_ReceiveMessage(int32_t busId, uint32_t messageId,
                            struct HAL_CAN2027ReceiveMessage* message,
                            int32_t* status);
/**
 * Gets CAN status information.
 *
 * @param[in]  busId                 the bus number
 * @param[out] percentBusUtilization the bus utilization
 * @param[out] busOffCount           the number of bus off errors
 * @param[out] txFullCount           the number of tx full errors
 * @param[out] receiveErrorCount     the number of receive errors
 * @param[out] transmitErrorCount    the number of transmit errors
 * @param[out] status                Error status variable. 0 on success.
 */
void HAL_CAN2027_GetCANStatus(int32_t busId, float* percentBusUtilization,
                          uint32_t* busOffCount, uint32_t* txFullCount,
                          uint32_t* receiveErrorCount,
                          uint32_t* transmitErrorCount, int32_t* status);
/** @} */
/**
 * @ingroup hal_canstream
 * @{
 */
/**
 * Opens a CAN stream.
 *
 * @param[in]  busId                   the bus number
 * @param[in] messageId     the message ID to read
 * @param[in] messageIDMask the message ID mask
 * @param[in] maxMessages   the maximum number of messages to stream
 * @param[out] status        Error status variable. 0 on success.
 * @return  the stream handle
 */
HAL_CAN2027StreamHandle HAL_CAN2027_OpenStreamSession(int32_t busId, uint32_t messageId,
                                              uint32_t messageIDMask,
                                              uint32_t maxMessages,
                                              int32_t* status);

/**
 * Closes a CAN stream.
 *
 * @param sessionHandle the session to close
 */
void HAL_CAN2027_CloseStreamSession(HAL_CAN2027StreamHandle sessionHandle);

/**
 * Reads a CAN stream message.
 *
 * @param[in] sessionHandle  the session handle
 * @param[in] messages       array of messages
 * @param[in] messagesToRead the max number of messages to read
 * @param[out] messagesRead   the number of messages actually read
 * @param[out] status         Error status variable. 0 on success.
 */
void HAL_CAN2027_ReadStreamSession(HAL_CAN2027StreamHandle sessionHandle,
                               struct HAL_CAN2027StreamMessage* messages,
                               uint32_t messagesToRead, uint32_t* messagesRead,
                               int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */

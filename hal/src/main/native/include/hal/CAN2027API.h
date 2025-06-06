// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/CAN2027.h"
#include "hal/CANAPITypes.h"
#include "hal/CAN2027APITypes.h"
#include "hal/Types.h"

/**
 * @defgroup hal_canapi CAN API Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a CAN device.
 *
 * These follow the FIRST standard CAN layout.
 * https://docs.wpilib.org/en/stable/docs/software/can-devices/can-addressing.html
 *
 * @param[in] busId        the bus id
 * @param[in] manufacturer the can manufacturer
 * @param[in] deviceId     the device ID (0-63)
 * @param[in] deviceType   the device type
 * @param[out] status      Error status variable. 0 on success.
 * @return the created CAN handle
 */
HAL_CANHandle HAL_InitializeCAN2027(int32_t busId, HAL_CANManufacturer manufacturer,
                                int32_t deviceId, HAL_CANDeviceType deviceType,
                                int32_t* status);

/**
 * Frees a CAN device
 *
 * @param handle the CAN handle
 */
void HAL_CleanCAN2027(HAL_CANHandle handle);

/**
 * Writes a packet to the CAN device with a specific ID.
 *
 * This ID is 10 bits.
 *
 * @param[in] handle  the CAN handle
 * @param[in] apiId   the ID to write (0-1023)
 * @param[in] message the message
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_WriteCAN2027Packet(HAL_CANHandle handle, int32_t apiId,
                        const struct HAL_CAN2027Message* message, int32_t* status);

/**
 * Writes a repeating packet to the CAN device with a specific ID.
 *
 * This ID is 10 bits.
 *
 * The device will automatically repeat the packet at the specified interval
 *
 * @param[in] handle   the CAN handle
 * @param[in] apiId    the ID to write (0-1023)
 * @param[in] message  the message
 * @param[in] repeatMs the period to repeat in ms
 * @param[out] status  Error status variable. 0 on success.
 */
void HAL_WriteCAN2027PacketRepeating(HAL_CANHandle handle, int32_t apiId,
                                 const struct HAL_CAN2027Message* message,
                                 int32_t repeatMs, int32_t* status);

/**
 * Writes an RTR frame of the specified length to the CAN device with the
 * specific ID.
 *
 * By spec, the length must be equal to the length sent by the other device,
 * otherwise behavior is unspecified.
 *
 * @param[in] handle   the CAN handle
 * @param[in] apiId    the ID to write (0-1023)
 * @param[in] message  the message
 * @param[out] status  Error status variable. 0 on success.
 */
void HAL_WriteCAN2027RTRFrame(HAL_CANHandle handle, int32_t apiId,
                          const struct HAL_CAN2027Message* message,
                          int32_t* status);

/**
 * Stops a repeating packet with a specific ID.
 *
 * This ID is 10 bits.
 *
 * @param[in] handle  the CAN handle
 * @param[in] apiId   the ID to stop repeating (0-1023)
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_StopCAN2027PacketRepeating(HAL_CANHandle handle, int32_t apiId,
                                int32_t* status);

/**
 * Reads a new CAN packet.
 *
 * This will only return properly once per packet received. Multiple calls
 * without receiving another packet will return an error code.
 *
 * @param[in] handle    the CAN handle
 * @param[in] apiId     the ID to read (0-1023)
 * @param[out] message  the message received.
 * @param[out] status   Error status variable. 0 on success.
 */
void HAL_ReadCAN2027PacketNew(HAL_CANHandle handle, int32_t apiId,
                          struct HAL_CAN2027ReceiveMessage* message,
                          int32_t* status);

/**
 * Reads a CAN packet. The will continuously return the last packet received,
 * without accounting for packet age.
 *
 * @param[in] handle    the CAN handle
 * @param[in] apiId     the ID to read (0-1023)
 * @param[out] message  the message received.
 * @param[out] status   Error status variable. 0 on success.
 */
void HAL_ReadCAN2027PacketLatest(HAL_CANHandle handle, int32_t apiId,
                             struct HAL_CAN2027ReceiveMessage* message,
                             int32_t* status);

/**
 * Reads a CAN packet. The will return the last packet received until the
 * packet is older then the requested timeout. Then it will return an error
 * code.
 *
 * @param[in] handle        the CAN handle
 * @param[in] apiId         the ID to read (0-1023)
 * @param[out] message      the message received.
 * @param[out] timeoutMs    the timeout time for the packet
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_ReadCAN2027PacketTimeout(HAL_CANHandle handle, int32_t apiId,
                              struct HAL_CAN2027ReceiveMessage* message,
                              int32_t timeoutMs, int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */

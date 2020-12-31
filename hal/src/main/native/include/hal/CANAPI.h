// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/CANAPITypes.h"
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
 * These follow the FIRST standard CAN layout. Link TBD
 *
 * @param manufacturer the can manufacturer
 * @param deviceId     the device ID (0-63)
 * @param deviceType   the device type
 * @return             the created CAN handle
 */
HAL_CANHandle HAL_InitializeCAN(HAL_CANManufacturer manufacturer,
                                int32_t deviceId, HAL_CANDeviceType deviceType,
                                int32_t* status);

/**
 * Frees a CAN device
 *
 * @param handle the CAN handle
 */
void HAL_CleanCAN(HAL_CANHandle handle);

/**
 * Writes a packet to the CAN device with a specific ID.
 *
 * This ID is 10 bits.
 *
 * @param handle the CAN handle
 * @param data   the data to write (0-8 bytes)
 * @param length the length of data (0-8)
 * @param apiId  the ID to write (0-1023 bits)
 */
void HAL_WriteCANPacket(HAL_CANHandle handle, const uint8_t* data,
                        int32_t length, int32_t apiId, int32_t* status);

/**
 * Writes a repeating packet to the CAN device with a specific ID.
 *
 * This ID is 10 bits.
 *
 * The RoboRIO will automatically repeat the packet at the specified interval
 *
 * @param handle   the CAN handle
 * @param data     the data to write (0-8 bytes)
 * @param length   the length of data (0-8)
 * @param apiId    the ID to write (0-1023)
 * @param repeatMs the period to repeat in ms
 */
void HAL_WriteCANPacketRepeating(HAL_CANHandle handle, const uint8_t* data,
                                 int32_t length, int32_t apiId,
                                 int32_t repeatMs, int32_t* status);

/**
 * Writes an RTR frame of the specified length to the CAN device with the
 * specific ID.
 *
 * By spec, the length must be equal to the length sent by the other device,
 * otherwise behavior is unspecified.
 *
 * @param handle   the CAN handle
 * @param length   the length of data to request (0-8)
 * @param apiId    the ID to write (0-1023)
 */
void HAL_WriteCANRTRFrame(HAL_CANHandle handle, int32_t length, int32_t apiId,
                          int32_t* status);

/**
 * Stops a repeating packet with a specific ID.
 *
 * This ID is 10 bits.
 *
 * @param handle the CAN handle
 * @param apiId  the ID to stop repeating (0-1023)
 */
void HAL_StopCANPacketRepeating(HAL_CANHandle handle, int32_t apiId,
                                int32_t* status);

/**
 * Reads a new CAN packet.
 *
 * This will only return properly once per packet received. Multiple calls
 * without receiving another packet will return an error code.
 *
 * @param handle            the CAN handle
 * @param apiId             the ID to read (0-1023)
 * @param data              the packet data (8 bytes)
 * @param length            the received length (0-8 bytes)
 * @param receivedTimestamp the packet received timestamp (based off of
 * CLOCK_MONOTONIC)
 */
void HAL_ReadCANPacketNew(HAL_CANHandle handle, int32_t apiId, uint8_t* data,
                          int32_t* length, uint64_t* receivedTimestamp,
                          int32_t* status);

/**
 * Reads a CAN packet. The will continuously return the last packet received,
 * without accounting for packet age.
 *
 * @param handle            the CAN handle
 * @param apiId             the ID to read (0-1023)
 * @param data              the packet data (8 bytes)
 * @param length            the received length (0-8 bytes)
 * @param receivedTimestamp the packet received timestamp (based off of
 * CLOCK_MONOTONIC)
 */
void HAL_ReadCANPacketLatest(HAL_CANHandle handle, int32_t apiId, uint8_t* data,
                             int32_t* length, uint64_t* receivedTimestamp,
                             int32_t* status);

/**
 * Reads a CAN packet. The will return the last packet received until the
 * packet is older then the requested timeout. Then it will return an error
 * code.
 *
 * @param handle            the CAN handle
 * @param apiId             the ID to read (0-1023)
 * @param data              the packet data (8 bytes)
 * @param length            the received length (0-8 bytes)
 * @param receivedTimestamp the packet received timestamp (based off of
 * CLOCK_MONOTONIC)
 * @param timeoutMs         the timeout time for the packet
 */
void HAL_ReadCANPacketTimeout(HAL_CANHandle handle, int32_t apiId,
                              uint8_t* data, int32_t* length,
                              uint64_t* receivedTimestamp, int32_t timeoutMs,
                              int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */

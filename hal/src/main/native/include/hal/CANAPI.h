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
 * Reads the current value of the millisecond-resolution timer that the CAN API
 * functions use as a time base.
 *
 * @return Current value of timer used as a base time by the CAN API in
 * milliseconds.
 */
uint32_t HAL_GetCANPacketBaseTime(void);

/**
 * Initializes a CAN device.
 *
 * These follow the FIRST standard CAN layout.
 * https://docs.wpilib.org/en/stable/docs/software/can-devices/can-addressing.html
 *
 * @param[in] manufacturer the can manufacturer
 * @param[in] deviceId     the device ID (0-63)
 * @param[in] deviceType   the device type
 * @param[out] status      Error status variable. 0 on success.
 * @return the created CAN handle
 */
HAL_CANHandle HAL_InitializeCAN(HAL_CANManufacturer manufacturer,
                                int32_t deviceId, HAL_CANDeviceType deviceType,
                                int32_t* status);

/**
 * Frees a CAN device
 *
 * @param[in,out] handle the CAN handle
 */
void HAL_CleanCAN(HAL_CANHandle handle);

/**
 * Writes a packet to the CAN device with a specific ID.
 *
 * This ID is 10 bits.
 *
 * @param[in] handle  the CAN handle
 * @param[in] data    the data to write (0-8 bytes)
 * @param[in] length  the length of data (0-8)
 * @param[in] apiId   the ID to write (0-1023 bits)
 * @param[out] status Error status variable. 0 on success.
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
 * @param[in] handle   the CAN handle
 * @param[in] data     the data to write (0-8 bytes)
 * @param[in] length   the length of data (0-8)
 * @param[in] apiId    the ID to write (0-1023)
 * @param[in] repeatMs the period to repeat in ms
 * @param[out] status  Error status variable. 0 on success.
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
 * @param[in] handle   the CAN handle
 * @param[in] length   the length of data to request (0-8)
 * @param[in] apiId    the ID to write (0-1023)
 * @param[out] status  Error status variable. 0 on success.
 */
void HAL_WriteCANRTRFrame(HAL_CANHandle handle, int32_t length, int32_t apiId,
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
void HAL_StopCANPacketRepeating(HAL_CANHandle handle, int32_t apiId,
                                int32_t* status);

/**
 * Reads a new CAN packet.
 *
 * This will only return properly once per packet received. Multiple calls
 * without receiving another packet will return an error code.
 *
 * @param[in] handle             the CAN handle
 * @param[in] apiId              the ID to read (0-1023)
 * @param[out] data              the packet data (8 bytes)
 * @param[out] length            the received length (0-8 bytes)
 * @param[out] receivedTimestamp the packet received timestamp in ms (based off
 *                               of CLOCK_MONOTONIC)
 * @param[out] status            Error status variable. 0 on success.
 */
void HAL_ReadCANPacketNew(HAL_CANHandle handle, int32_t apiId, uint8_t* data,
                          int32_t* length, uint64_t* receivedTimestamp,
                          int32_t* status);

/**
 * Reads a CAN packet. The will continuously return the last packet received,
 * without accounting for packet age.
 *
 * @param[in] handle             the CAN handle
 * @param[in] apiId              the ID to read (0-1023)
 * @param[out] data              the packet data (8 bytes)
 * @param[out] length            the received length (0-8 bytes)
 * @param[out] receivedTimestamp the packet received timestamp in ms (based off
 *                               of CLOCK_MONOTONIC)
 * @param[out] status            Error status variable. 0 on success.
 */
void HAL_ReadCANPacketLatest(HAL_CANHandle handle, int32_t apiId, uint8_t* data,
                             int32_t* length, uint64_t* receivedTimestamp,
                             int32_t* status);

/**
 * Reads a CAN packet. The will return the last packet received until the
 * packet is older then the requested timeout. Then it will return an error
 * code.
 *
 * @param[in] handle             the CAN handle
 * @param[in] apiId              the ID to read (0-1023)
 * @param[out] data              the packet data (8 bytes)
 * @param[out] length            the received length (0-8 bytes)
 * @param[out] receivedTimestamp the packet received timestamp in ms (based off
 *                               of CLOCK_MONOTONIC)
 * @param[out] timeoutMs         the timeout time for the packet
 * @param[out] status            Error status variable. 0 on success.
 */
void HAL_ReadCANPacketTimeout(HAL_CANHandle handle, int32_t apiId,
                              uint8_t* data, int32_t* length,
                              uint64_t* receivedTimestamp, int32_t timeoutMs,
                              int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */

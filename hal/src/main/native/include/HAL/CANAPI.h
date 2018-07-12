/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/Types.h"

// clang-format off
/**
 * The CAN device type.
 *
 * Teams should use HAL_CAN_Dev_kMiscellaneous
 */
HAL_ENUM(HAL_CANDeviceType) {
  HAL_CAN_Dev_kBroadcast = 0,
  HAL_CAN_Dev_kRobotController = 1,
  HAL_CAN_Dev_kMotorController = 2,
  HAL_CAN_Dev_kRelayController = 3,
  HAL_CAN_Dev_kGyroSensor = 4,
  HAL_CAN_Dev_kAccelerometer = 5,
  HAL_CAN_Dev_kUltrasonicSensor = 6,
  HAL_CAN_Dev_kGearToothSensor = 7,
  HAL_CAN_Dev_kPowerDistribution = 8,
  HAL_CAN_Dev_kPneumatics = 9,
  HAL_CAN_Dev_kMiscellaneous = 10,
  HAL_CAN_Dev_kFirmwareUpdate = 31
};

/**
 * The CAN manufacturer ID.
 *
 * Teams should use HAL_CAN_Man_kTeamUse.
 */
HAL_ENUM(HAL_CANManufacturer) {
  HAL_CAN_Man_kBroadcast = 0,
  HAL_CAN_Man_kNI = 1,
  HAL_CAN_Man_kLM = 2,
  HAL_CAN_Man_kDEKA = 3,
  HAL_CAN_Man_kCTRE = 4,
  HAL_CAN_Man_kMS = 7,
  HAL_CAN_Man_kTeamUse = 8,
};
// clang-format on

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

/**
 * Reads a CAN packet. The will return the last packet received until the
 * packet is older then the requested timeout. Then it will return an error
 * code. The period parameter is used when you know the packet is sent at
 * specific intervals, so calls will not attempt to read a new packet from the
 * network until that period has passed. We do not recommend users use this
 * API unless they know the implications.
 *
 * @param handle            the CAN handle
 * @param apiId             the ID to read (0-1023)
 * @param data              the packet data (8 bytes)
 * @param length            the received length (0-8 bytes)
 * @param receivedTimestamp the packet received timestamp (based off of
 * CLOCK_MONOTONIC)
 * @param timeoutMs         the timeout time for the packet
 * @param periodMs          the standard period for the packet
 */
void HAL_ReadCANPeriodicPacket(HAL_CANHandle handle, int32_t apiId,
                               uint8_t* data, int32_t* length,
                               uint64_t* receivedTimestamp, int32_t timeoutMs,
                               int32_t periodMs, int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif

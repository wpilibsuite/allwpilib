/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/Types.h"

enum HAL_CANDeviceType : int32_t {
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

enum HAL_CANManufacturer : int32_t {
  HAL_CAN_Man_kBroadcast = 0,
  HAL_CAN_Man_kNI = 1,
  HAL_CAN_Man_kLM = 2,
  HAL_CAN_Man_kDEKA = 3,
  HAL_CAN_Man_kCTRE = 4,
  HAL_CAN_Man_kMS = 7,
  HAL_CAN_Man_kTeamUse = 8,
};

#ifdef __cplusplus
extern "C" {
#endif

HAL_CANHandle HAL_InitializeCAN(HAL_CANManufacturer manufacturer,
                                int32_t deviceId, HAL_CANDeviceType deviceType,
                                int32_t* status);

void HAL_CleanCAN(HAL_CANHandle handle);

void HAL_WriteCANPacket(HAL_CANHandle handle, const uint8_t* data,
                        int32_t length, int32_t apiId, int32_t* status);

void HAL_WriteCANPacketRepeating(HAL_CANHandle handle, const uint8_t* data,
                                 int32_t length, int32_t apiId,
                                 int32_t repeatMs, int32_t* status);

void HAL_StopCANPacketRepeating(HAL_CANHandle handle, int32_t apiId,
                                int32_t* status);

void HAL_ReadCANPacketNew(HAL_CANHandle handle, int32_t apiId, uint8_t* data,
                          int32_t* length, uint64_t* receivedTimestamp,
                          int32_t* status);

void HAL_ReadCANPacketLatest(HAL_CANHandle handle, int32_t apiId, uint8_t* data,
                             int32_t* length, uint64_t* receivedTimestamp,
                             int32_t* status);

void HAL_ReadCANPacketTimeout(HAL_CANHandle handle, int32_t apiId,
                              uint8_t* data, int32_t* length,
                              uint64_t* receivedTimestamp, int32_t timeoutMs,
                              int32_t* status);
void HAL_ReadCANPeriodicPacket(HAL_CANHandle handle, int32_t apiId,
                               uint8_t* data, int32_t* length,
                               uint64_t* receivedTimestamp, int32_t timeoutMs,
                               int32_t periodMs, int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif

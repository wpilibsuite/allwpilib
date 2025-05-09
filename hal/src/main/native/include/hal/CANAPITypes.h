// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_canapi CAN API Functions
 * @ingroup hal_capi
 * @{
 */

/**
 * The CAN device type.
 *
 * Teams should use HAL_CAN_Dev_Miscellaneous
 */
HAL_ENUM(HAL_CANDeviceType) {
  /// Broadcast.
  HAL_CAN_Dev_Broadcast = 0,
  /// Robot controller.
  HAL_CAN_Dev_RobotController = 1,
  /// Motor controller.
  HAL_CAN_Dev_MotorController = 2,
  /// Relay controller.
  HAL_CAN_Dev_RelayController = 3,
  /// Gyro sensor.
  HAL_CAN_Dev_GyroSensor = 4,
  /// Accelerometer.
  HAL_CAN_Dev_Accelerometer = 5,
  /// Ultrasonic sensor.
  HAL_CAN_Dev_UltrasonicSensor = 6,
  /// Gear tooth sensor.
  HAL_CAN_Dev_GearToothSensor = 7,
  /// Power distribution.
  HAL_CAN_Dev_PowerDistribution = 8,
  /// Pneumatics.
  HAL_CAN_Dev_Pneumatics = 9,
  /// Miscellaneous.
  HAL_CAN_Dev_Miscellaneous = 10,
  /// IO breakout.
  HAL_CAN_Dev_IOBreakout = 11,
  // Servo controller.
  HAL_CAN_Dev_ServoController = 12,
  /// Firmware update.
  HAL_CAN_Dev_FirmwareUpdate = 31
};

/**
 * The CAN manufacturer ID.
 *
 * Teams should use HAL_CAN_Man_TeamUse.
 */
HAL_ENUM(HAL_CANManufacturer) {
  /// Broadcast.
  HAL_CAN_Man_Broadcast = 0,
  /// National Instruments.
  HAL_CAN_Man_NI = 1,
  /// Luminary Micro.
  HAL_CAN_Man_LM = 2,
  /// DEKA Research and Development Corp.
  HAL_CAN_Man_DEKA = 3,
  /// Cross the Road Electronics.
  HAL_CAN_Man_CTRE = 4,
  /// REV robotics.
  HAL_CAN_Man_REV = 5,
  /// Grapple.
  HAL_CAN_Man_Grapple = 6,
  /// MindSensors.
  HAL_CAN_Man_MS = 7,
  /// Team use.
  HAL_CAN_Man_TeamUse = 8,
  /// Kauai Labs.
  HAL_CAN_Man_KauaiLabs = 9,
  /// Copperforge.
  HAL_CAN_Man_Copperforge = 10,
  /// Playing With Fusion.
  HAL_CAN_Man_PWF = 11,
  /// Studica.
  HAL_CAN_Man_Studica = 12,
  /// TheThriftyBot.
  HAL_CAN_Man_TheThriftyBot = 13,
  /// Redux Robotics.
  HAL_CAN_Man_ReduxRobotics = 14,
  /// AndyMark.
  HAL_CAN_Man_AndyMark = 15,
  /// Vivid-Hosting.
  HAL_CAN_Man_VividHosting = 16
};

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
#define HAL_CAN_IS_FRAME_REMOTE 0x40000000

/**
 * Mask for "is frame 11 bits" in message ID.
 */
#define HAL_CAN_IS_FRAME_11BIT 0x80000000

HAL_ENUM(HAL_CANFlags) {
  /** Placeholder for no flags */
  HAL_CAN_NO_FLAGS = 0x0,
  /**
   * Mask for if frame will do FD bit rate switching.
   * Only matters to send.
   */
  HAL_CAN_FD_BITRATESWITCH = 0x1,
  /**
   * Mask for is frame will contain an FD length.
   */
  HAL_CAN_FD_DATALENGTH = 0x2,
};

struct HAL_CANMessage {
  /** Flags for the message (HAL_CANFlags) */
  int32_t flags;
  /** The size of the data received (0-64 bytes) */
  uint8_t dataSize;
  /** The message data */
  uint8_t data[64];
};

struct HAL_CANReceiveMessage {
  /** Receive timestamp (wpi time) */
  uint64_t timeStamp;
  /** The received message */
  struct HAL_CANMessage message;
};
/** @} */

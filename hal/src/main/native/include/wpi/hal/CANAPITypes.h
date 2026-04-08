// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/Types.h"

/**
 * @defgroup hal_canapi CAN API Functions
 * @ingroup hal_capi
 * @{
 */

/**
 * The CAN device type.
 *
 * Teams should use HAL_CAN_DEV_MISCELLANEOUS
 */
HAL_ENUM(HAL_CANDeviceType) {
  /// Broadcast.
  HAL_CAN_DEV_BROADCAST = 0,
  /// Robot controller.
  HAL_CAN_DEV_ROBOT_CONTROLLER = 1,
  /// Motor controller.
  HAL_CAN_DEV_MOTOR_CONTROLLER = 2,
  /// Relay controller.
  HAL_CAN_DEV_RELAY_CONTROLLER = 3,
  /// Gyro sensor.
  HAL_CAN_DEV_GYRO_SENSOR = 4,
  /// Accelerometer.
  HAL_CAN_DEV_ACCELEROMETER = 5,
  /// Distance sensor.
  HAL_CAN_DEV_DISTANCE_SENSOR = 6,
  /// Encoder.
  HAL_CAN_DEV_ENCODER = 7,
  /// Power distribution.
  HAL_CAN_DEV_POWER_DISTRIBUTION = 8,
  /// Pneumatics.
  HAL_CAN_DEV_PNEUMATICS = 9,
  /// Miscellaneous.
  HAL_CAN_DEV_MISCELLANEOUS = 10,
  /// IO breakout.
  HAL_CAN_DEV_IO_BREAKOUT = 11,
  /// Servo controller.
  HAL_CAN_DEV_SERVO_CONTROLLER = 12,
  /// Color Sensor.
  HAL_CAN_DEV_COLOR_SENSOR = 13,
  /// Firmware update.
  HAL_CAN_DEV_FIRMWARE_UPDATE = 31
};

/**
 * The CAN manufacturer ID.
 *
 * Teams should use HAL_CAN_MAN_TEAM_USE.
 */
HAL_ENUM(HAL_CANManufacturer) {
  /// Broadcast.
  HAL_CAN_MAN_BROADCAST = 0,
  /// National Instruments.
  HAL_CAN_MAN_NI = 1,
  /// Luminary Micro.
  HAL_CAN_MAN_LM = 2,
  /// DEKA Research and Development Corp.
  HAL_CAN_MAN_DEKA = 3,
  /// Cross the Road Electronics.
  HAL_CAN_MAN_CTRE = 4,
  /// REV robotics.
  HAL_CAN_MAN_REV = 5,
  /// Grapple.
  HAL_CAN_MAN_GRAPPLE = 6,
  /// MindSensors.
  HAL_CAN_MAN_MS = 7,
  /// Team use.
  HAL_CAN_MAN_TEAM_USE = 8,
  /// Kauai Labs.
  HAL_CAN_MAN_KAUAI_LABS = 9,
  /// Copperforge.
  HAL_CAN_MAN_COPPERFORGE = 10,
  /// Playing With Fusion.
  HAL_CAN_MAN_PWF = 11,
  /// Studica.
  HAL_CAN_MAN_STUDICA = 12,
  /// TheThriftyBot.
  HAL_CAN_MAN_THRIFTYBOT = 13,
  /// Redux Robotics.
  HAL_CAN_MAN_REDUX_ROBOTICS = 14,
  /// AndyMark.
  HAL_CAN_MAN_ANDYMARK = 15,
  /// Vivid-Hosting.
  HAL_CAN_MAN_VIVID_HOSTING = 16,
  /// Vertos Robotics.
  HAL_CAN_MAN_VERTOS_ROBOTICS = 17,
  /// SWYFT Robotics.
  HAL_CAN_MAN_SWYFT_ROBOTICS = 18,
  /// Lumyn Labs.
  HAL_CAN_MAN_LUMYN_LABS = 19,
  /// Brushland Labs
  HAL_CAN_MAN_BRUSHLAND_LABS = 20
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

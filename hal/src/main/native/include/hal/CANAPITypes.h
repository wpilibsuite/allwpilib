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
 * Teams should use HAL_CAN_Dev_kMiscellaneous
 */
HAL_ENUM(HAL_CANDeviceType) {
  /// Broadcast.
  HAL_CAN_Dev_kBroadcast = 0,
  /// Robot controller.
  HAL_CAN_Dev_kRobotController = 1,
  /// Motor controller.
  HAL_CAN_Dev_kMotorController = 2,
  /// Relay controller.
  HAL_CAN_Dev_kRelayController = 3,
  /// Gyro sensor.
  HAL_CAN_Dev_kGyroSensor = 4,
  /// Accelerometer.
  HAL_CAN_Dev_kAccelerometer = 5,
  /// Ultrasonic sensor.
  HAL_CAN_Dev_kUltrasonicSensor = 6,
  /// Gear tooth sensor.
  HAL_CAN_Dev_kGearToothSensor = 7,
  /// Power distribution.
  HAL_CAN_Dev_kPowerDistribution = 8,
  /// Pneumatics.
  HAL_CAN_Dev_kPneumatics = 9,
  /// Miscellaneous.
  HAL_CAN_Dev_kMiscellaneous = 10,
  /// IO breakout.
  HAL_CAN_Dev_kIOBreakout = 11,
  // Servo controller.
  HAL_CAN_Dev_kServoController = 12,
  /// Firmware update.
  HAL_CAN_Dev_kFirmwareUpdate = 31
};

/**
 * The CAN manufacturer ID.
 *
 * Teams should use HAL_CAN_Man_kTeamUse.
 */
HAL_ENUM(HAL_CANManufacturer) {
  /// Broadcast.
  HAL_CAN_Man_kBroadcast = 0,
  /// National Instruments.
  HAL_CAN_Man_kNI = 1,
  /// Luminary Micro.
  HAL_CAN_Man_kLM = 2,
  /// DEKA Research and Development Corp.
  HAL_CAN_Man_kDEKA = 3,
  /// Cross the Road Electronics.
  HAL_CAN_Man_kCTRE = 4,
  /// REV robotics.
  HAL_CAN_Man_kREV = 5,
  /// Grapple.
  HAL_CAN_Man_kGrapple = 6,
  /// MindSensors.
  HAL_CAN_Man_kMS = 7,
  /// Team use.
  HAL_CAN_Man_kTeamUse = 8,
  /// Kauai Labs.
  HAL_CAN_Man_kKauaiLabs = 9,
  /// Copperforge.
  HAL_CAN_Man_kCopperforge = 10,
  /// Playing With Fusion.
  HAL_CAN_Man_kPWF = 11,
  /// Studica.
  HAL_CAN_Man_kStudica = 12,
  /// TheThriftyBot.
  HAL_CAN_Man_kTheThriftyBot = 13,
  /// Redux Robotics.
  HAL_CAN_Man_kReduxRobotics = 14,
  /// AndyMark.
  HAL_CAN_Man_kAndyMark = 15,
  /// Vivid-Hosting.
  HAL_CAN_Man_kVividHosting = 16
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

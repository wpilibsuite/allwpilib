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
/** @} */

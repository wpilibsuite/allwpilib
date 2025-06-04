// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * Flag for sending a CAN message once.
 */
#define HAL_CAN2027_SEND_PERIOD_NO_REPEAT 0

/**
 * Flag for stopping periodic CAN message sends.
 */
#define HAL_CAN2027_SEND_PERIOD_STOP_REPEATING -1

/**
 * Mask for "is frame remote" in message ID.
 */
#define HAL_CAN2027_IS_FRAME_REMOTE 0x40000000

/**
 * Mask for "is frame 11 bits" in message ID.
 */
#define HAL_CAN2027_IS_FRAME_11BIT 0x80000000

HAL_ENUM(HAL_CAN2027Flags) {
  /** Placeholder for no flags */
  HAL_CAN2027_NO_FLAGS = 0x0,
  /**
   * Mask for if frame will do FD bit rate switching.
   * Only matters to send.
   */
  HAL_CAN2027_FD_BITRATESWITCH = 0x1,
  /**
   * Mask for is frame will contain an FD length.
   */
  HAL_CAN2027_FD_DATALENGTH = 0x2,
};

struct HAL_CAN2027Message {
  /** Flags for the message (HAL_CANFlags) */
  int32_t flags;
  /** The size of the data received (0-64 bytes) */
  uint8_t dataSize;
  /** The message data */
  uint8_t data[64];
};

struct HAL_CAN2027ReceiveMessage {
  /** Receive timestamp (wpi time) */
  uint64_t timeStamp;
  /** The received message */
  struct HAL_CAN2027Message message;
};
/** @} */

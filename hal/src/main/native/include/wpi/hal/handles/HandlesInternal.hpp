// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/Types.h"
#include "wpi/util/Synchronization.hpp"

/* General Handle Data Layout
 * Bits 0-15:  Handle Index
 * Bits 16-23: 8 bit rolling reset detection
 * Bits 24-30: Handle Type
 * Bit 31:     1 if handle error, 0 if no error
 *
 * Other specialized handles will use different formats, however Bits 24-31 are
 * always reserved for type and error handling.
 */

namespace wpi::hal {

/**
 * Base for all HAL Handles.
 */
class HandleBase {
 public:
  HandleBase();
  ~HandleBase();
  HandleBase(const HandleBase&) = delete;
  HandleBase& operator=(const HandleBase&) = delete;
  virtual void ResetHandles();
  static void ResetGlobalHandles();

 protected:
  int16_t m_version;
};

constexpr int16_t INVALID_HANDLE_INDEX = -1;

/**
 * Enum of HAL handle types. Vendors/Teams should use Vendor (17).
 */
enum class HAL_HandleEnum {
  UNDEFINED = 0,
  DIO = wpi::util::HANDLE_TYPE_HAL_BASE,
  PORT = 2,
  NOTIFIER = 3,
  INTERRUPT = 4,
  ANALOG_OUTPUT = 5,
  ANALOG_INPUT = 6,
  ANALOG_TRIGGER = 7,
  RELAY = 8,
  PWM = 9,
  DIGITAL_PWM = 10,
  COUNTER = 11,
  FPGA_ENCODER = 12,
  ENCODER = 13,
  COMPRESSOR = 14,
  SOLENOID = 15,
  ANALOG_GYRO = 16,
  VENDOR = 17,
  SIMULATION_JNI = 18,
  CAN = 19,
  SERIAL_PORT = 20,
  DUTY_CYCLE = 21,
  DMA = 22,
  ADDRESSABLE_LED = 23,
  CTRE_PCM = 24,
  CTRE_PDP = 25,
  REV_PDH = 26,
  REV_PH = 27,
  CAN_STREAM = 28,
  ALERT = 29,
};

/**
 * Get the handle index from a handle.
 *
 * @param handle the handle
 * @return the index
 */
static inline int16_t getHandleIndex(HAL_Handle handle) {
  // mask and return last 16 bits
  return static_cast<int16_t>(handle & 0xffff);
}

/**
 * Get the handle type from a handle.
 *
 * @param handle the handle
 * @return the type
 */
static inline HAL_HandleEnum getHandleType(HAL_Handle handle) {
  // mask first 8 bits and cast to enum
  return static_cast<HAL_HandleEnum>((handle >> 24) & 0xff);
}

/**
 * Get if the handle is a specific type.
 *
 * @param handle     the handle
 * @param handleType the type to check
 * @return true if the type is correct, otherwise false
 */
static inline bool isHandleType(HAL_Handle handle, HAL_HandleEnum handleType) {
  return handleType == getHandleType(handle);
}

/**
 * Get if the version of the handle is correct.
 *
 * Do not use on the roboRIO, used specifically for the sim to handle resets.
 *
 * @param handle  the handle
 * @param version the handle version to check
 * @return true if the handle is the right version, otherwise false
 */
static inline bool isHandleCorrectVersion(HAL_Handle handle, int16_t version) {
  return (((handle & 0xFF0000) >> 16) & version) == version;
}

/**
 * Get if the handle is a correct type and version.
 *
 * Note the version is not checked on the roboRIO.
 *
 * @param handle     the handle
 * @param enumType   the type to check
 * @param version    the handle version to check
 * @return true if the handle is proper version and type, otherwise
 *         false.
 */
inline int16_t getHandleTypedIndex(HAL_Handle handle, HAL_HandleEnum enumType,
                                   int16_t version) {
  if (!isHandleType(handle, enumType)) {
    return INVALID_HANDLE_INDEX;
  }
#if !defined(__FIRST_SYSTEMCORE__)
  if (!isHandleCorrectVersion(handle, version)) {
    return INVALID_HANDLE_INDEX;
  }
#endif
  return getHandleIndex(handle);
}

/* specialized functions for Port handle
 * Port Handle Data Layout
 * Bits 0-7:   Channel Number
 * Bits 8-15:  Module Number
 * Bits 16-23: Unused
 * Bits 24-30: Handle Type
 * Bit 31:     1 if handle error, 0 if no error
 */

/**
 * Create a handle for a specific index, type and version.
 *
 * Note the version is not checked on the roboRIO.
 *
 * @param index      the index
 * @param handleType the handle type
 * @param version    the handle version
 * @return the created handle
 */
HAL_Handle createHandle(int16_t index, HAL_HandleEnum handleType,
                        int16_t version);
}  // namespace wpi::hal

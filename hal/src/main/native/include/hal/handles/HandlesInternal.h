// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/* General Handle Data Layout
 * Bits 0-15:  Handle Index
 * Bits 16-23: 8 bit rolling reset detection
 * Bits 24-30: Handle Type
 * Bit 31:     1 if handle error, 0 if no error
 *
 * Other specialized handles will use different formats, however Bits 24-31 are
 * always reserved for type and error handling.
 */

namespace hal {

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

constexpr int16_t InvalidHandleIndex = -1;

/**
 * Enum of HAL handle types. Vendors/Teams should use Vendor (17).
 */
enum class HAL_HandleEnum {
  Undefined = 0,
  DIO = 1,
  Port = 2,
  Notifier = 3,
  Interrupt = 4,
  AnalogOutput = 5,
  AnalogInput = 6,
  AnalogTrigger = 7,
  Relay = 8,
  PWM = 9,
  DigitalPWM = 10,
  Counter = 11,
  FPGAEncoder = 12,
  Encoder = 13,
  Compressor = 14,
  Solenoid = 15,
  AnalogGyro = 16,
  Vendor = 17,
  SimulationJni = 18,
  CAN = 19,
  SerialPort = 20,
  DutyCycle = 21,
  DMA = 22,
  AddressableLED = 23,
};

/**
 * Get the handle index from a handle.
 *
 * @param handle the handle
 * @return       the index
 */
static inline int16_t getHandleIndex(HAL_Handle handle) {
  // mask and return last 16 bits
  return static_cast<int16_t>(handle & 0xffff);
}

/**
 * Get the handle type from a handle.
 *
 * @param handle the handle
 * @return       the type
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
 * @return           true if the type is correct, otherwise false
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
 * @return        true if the handle is the right version, otherwise false
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
 * @param handleType the type to check
 * @param version    the handle version to check
 * @return           true if the handle is proper version and type, otherwise
 * false.
 */
inline int16_t getHandleTypedIndex(HAL_Handle handle, HAL_HandleEnum enumType,
                                   int16_t version) {
  if (!isHandleType(handle, enumType)) {
    return InvalidHandleIndex;
  }
#if !defined(__FRC_ROBORIO__)
  if (!isHandleCorrectVersion(handle, version)) {
    return InvalidHandleIndex;
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

// using a 16 bit value so we can store 0-255 and still report error
/**
 * Gets the port channel of a port handle.
 *
 * @param handle the port handle
 * @return       the port channel
 */
inline int16_t getPortHandleChannel(HAL_PortHandle handle) {
  if (!isHandleType(handle, HAL_HandleEnum::Port)) {
    return InvalidHandleIndex;
  }
  return static_cast<uint8_t>(handle & 0xff);
}

// using a 16 bit value so we can store 0-255 and still report error
/**
 * Gets the port module of a port handle.
 *
 * @param handle the port handle
 * @return       the port module
 */
inline int16_t getPortHandleModule(HAL_PortHandle handle) {
  if (!isHandleType(handle, HAL_HandleEnum::Port)) {
    return InvalidHandleIndex;
  }
  return static_cast<uint8_t>((handle >> 8) & 0xff);
}

// using a 16 bit value so we can store 0-255 and still report error
/**
 * Gets the SPI channel of a port handle.
 *
 * @param handle the port handle
 * @return       the port SPI channel
 */
inline int16_t getPortHandleSPIEnable(HAL_PortHandle handle) {
  if (!isHandleType(handle, HAL_HandleEnum::Port)) {
    return InvalidHandleIndex;
  }
  return static_cast<uint8_t>((handle >> 16) & 0xff);
}

/**
 * Create a port handle.
 *
 * @param channel the channel
 * @param module  the module
 * @return        port handle for the module and channel
 */
HAL_PortHandle createPortHandle(uint8_t channel, uint8_t module);

/**
 * Create a port handle for SPI.
 *
 * @param channel the SPI channel
 * @return        port handle for the channel
 */
HAL_PortHandle createPortHandleForSPI(uint8_t channel);

/**
 * Create a handle for a specific index, type and version.
 *
 * Note the version is not checked on the roboRIO.
 *
 * @param index      the index
 * @param handleType the handle type
 * @param version    the handle version
 * @return           the created handle
 */
HAL_Handle createHandle(int16_t index, HAL_HandleEnum handleType,
                        int16_t version);
}  // namespace hal

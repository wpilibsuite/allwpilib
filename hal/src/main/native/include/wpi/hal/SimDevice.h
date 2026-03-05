// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/Types.h"
#include "wpi/hal/Value.h"

/**
 * @defgroup hal_simdevice Simulator Device Framework
 * @ingroup hal_capi
 * HAL Simulator Device Framework.  This enables creating simulation-only
 * variables for higher level device access.  For example, a device such as
 * a SPI gyro can expose angle and rate variables to enable direct access
 * from simulation extensions or user test code instead of requiring that
 * the SPI bit-level protocol be implemented in simulation code.
 *
 * @{
 */

/**
 * Direction of a simulated value (from the perspective of user code).
 */
HAL_ENUM(HAL_SimValueDirection) {
  HAL_SimValueInput = 0, /**< input to user code from the simulator */
  HAL_SimValueOutput,    /**< output from user code to the simulator */
  HAL_SimValueBidir      /**< bidirectional between user code and simulator */
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates a simulated device.
 *
 * The device name must be unique.  0 is returned if the device name already
 * exists.  If multiple instances of the same device are desired, recommend
 * appending the instance/unique identifier in brackets to the base name,
 * e.g. "device[1]".
 *
 * Using a device name of the form "Type:Name" will create a WebSockets node
 * with a type value of "Type" and a device value of "Name"
 *
 * 0 is returned if not in simulation.
 *
 * @param name device name
 * @return simulated device handle
 */
HAL_SimDeviceHandle HAL_CreateSimDevice(const char* name);

/**
 * Frees a simulated device.
 *
 * This also allows the same device name to be used again.
 * This also frees all the simulated values created on the device.
 *
 * @param handle simulated device handle
 */
void HAL_FreeSimDevice(HAL_SimDeviceHandle handle);

/**
 * Get the name of a simulated device
 *
 * @param handle simulated device handle
 * @return name of the simulated device
 */
const char* HAL_GetSimDeviceName(HAL_SimDeviceHandle handle);

/**
 * Creates a value on a simulated device.
 *
 * Returns 0 if not in simulation; this can be used to avoid calls
 * to Set/Get functions.
 *
 * @param device simulated device handle
 * @param name value name
 * @param direction input/output/bidir (from perspective of user code)
 * @param initialValue initial value
 * @return simulated value handle
 */
HAL_SimValueHandle HAL_CreateSimValue(HAL_SimDeviceHandle device,
                                      const char* name, int32_t direction,
                                      const struct HAL_Value* initialValue);

/**
 * Creates an int value on a simulated device.
 *
 * Returns 0 if not in simulation; this can be used to avoid calls
 * to Set/Get functions.
 *
 * @param device simulated device handle
 * @param name value name
 * @param direction input/output/bidir (from perspective of user code)
 * @param initialValue initial value
 * @return simulated value handle
 */
inline HAL_SimValueHandle HAL_CreateSimValueInt(HAL_SimDeviceHandle device,
                                                const char* name,
                                                int32_t direction,
                                                int32_t initialValue) {
  struct HAL_Value v = HAL_MakeInt(initialValue);
  return HAL_CreateSimValue(device, name, direction, &v);
}

/**
 * Creates a long value on a simulated device.
 *
 * Returns 0 if not in simulation; this can be used to avoid calls
 * to Set/Get functions.
 *
 * @param device simulated device handle
 * @param name value name
 * @param direction input/output/bidir (from perspective of user code)
 * @param initialValue initial value
 * @return simulated value handle
 */
inline HAL_SimValueHandle HAL_CreateSimValueLong(HAL_SimDeviceHandle device,
                                                 const char* name,
                                                 int32_t direction,
                                                 int64_t initialValue) {
  struct HAL_Value v = HAL_MakeLong(initialValue);
  return HAL_CreateSimValue(device, name, direction, &v);
}

/**
 * Creates a double value on a simulated device.
 *
 * Returns 0 if not in simulation; this can be used to avoid calls
 * to Set/Get functions.
 *
 * @param device simulated device handle
 * @param name value name
 * @param direction input/output/bidir (from perspective of user code)
 * @param initialValue initial value
 * @return simulated value handle
 */
inline HAL_SimValueHandle HAL_CreateSimValueDouble(HAL_SimDeviceHandle device,
                                                   const char* name,
                                                   int32_t direction,
                                                   double initialValue) {
  struct HAL_Value v = HAL_MakeDouble(initialValue);
  return HAL_CreateSimValue(device, name, direction, &v);
}

/**
 * Creates an enumerated value on a simulated device.
 *
 * Enumerated values are always in the range 0 to numOptions-1.
 *
 * Returns 0 if not in simulation; this can be used to avoid calls
 * to Set/Get functions.
 *
 * @param device simulated device handle
 * @param name value name
 * @param direction input/output/bidir (from perspective of user code)
 * @param numOptions number of enumerated value options (length of options)
 * @param options array of option descriptions
 * @param initialValue initial value (selection)
 * @return simulated value handle
 */
HAL_SimValueHandle HAL_CreateSimValueEnum(HAL_SimDeviceHandle device,
                                          const char* name, int32_t direction,
                                          int32_t numOptions,
                                          const char** options,
                                          int32_t initialValue);

/**
 * Creates an enumerated value on a simulated device with double values.
 *
 * Enumerated values are always in the range 0 to numOptions-1.
 *
 * Returns 0 if not in simulation; this can be used to avoid calls
 * to Set/Get functions.
 *
 * @param device simulated device handle
 * @param name value name
 * @param direction input/output/bidir (from perspective of user code)
 * @param numOptions number of enumerated value options (length of options)
 * @param options array of option descriptions
 * @param optionValues array of option double values
 * @param initialValue initial value (selection)
 * @return simulated value handle
 */
HAL_SimValueHandle HAL_CreateSimValueEnumDouble(
    HAL_SimDeviceHandle device, const char* name, int32_t direction,
    int32_t numOptions, const char** options, const double* optionValues,
    int32_t initialValue);

/**
 * Creates a boolean value on a simulated device.
 *
 * Returns 0 if not in simulation; this can be used to avoid calls
 * to Set/Get functions.
 *
 * @param device simulated device handle
 * @param name value name
 * @param direction input/output/bidir (from perspective of user code)
 * @param initialValue initial value
 * @return simulated value handle
 */
inline HAL_SimValueHandle HAL_CreateSimValueBoolean(HAL_SimDeviceHandle device,
                                                    const char* name,
                                                    int32_t direction,
                                                    HAL_Bool initialValue) {
  struct HAL_Value v = HAL_MakeBoolean(initialValue);
  return HAL_CreateSimValue(device, name, direction, &v);
}

/**
 * Gets a simulated value.
 *
 * @param handle simulated value handle
 * @param value value (output parameter)
 */
void HAL_GetSimValue(HAL_SimValueHandle handle, struct HAL_Value* value);

/**
 * Gets a simulated value (int).
 *
 * @param handle simulated value handle
 * @return The current value
 */
inline int32_t HAL_GetSimValueInt(HAL_SimValueHandle handle) {
  struct HAL_Value v;
  HAL_GetSimValue(handle, &v);
  return v.type == HAL_INT ? v.data.v_int : 0;
}

/**
 * Gets a simulated value (long).
 *
 * @param handle simulated value handle
 * @return The current value
 */
inline int64_t HAL_GetSimValueLong(HAL_SimValueHandle handle) {
  struct HAL_Value v;
  HAL_GetSimValue(handle, &v);
  return v.type == HAL_LONG ? v.data.v_long : 0;
}

/**
 * Gets a simulated value (double).
 *
 * @param handle simulated value handle
 * @return The current value
 */
inline double HAL_GetSimValueDouble(HAL_SimValueHandle handle) {
  struct HAL_Value v;
  HAL_GetSimValue(handle, &v);
  return v.type == HAL_DOUBLE ? v.data.v_double : 0.0;
}

/**
 * Gets a simulated value (enum).
 *
 * @param handle simulated value handle
 * @return The current value
 */
inline int32_t HAL_GetSimValueEnum(HAL_SimValueHandle handle) {
  struct HAL_Value v;
  HAL_GetSimValue(handle, &v);
  return v.type == HAL_ENUM ? v.data.v_enum : 0;
}

/**
 * Gets a simulated value (boolean).
 *
 * @param handle simulated value handle
 * @return The current value
 */
inline HAL_Bool HAL_GetSimValueBoolean(HAL_SimValueHandle handle) {
  struct HAL_Value v;
  HAL_GetSimValue(handle, &v);
  return v.type == HAL_BOOLEAN ? v.data.v_boolean : 0;
}

/**
 * Sets a simulated value.
 *
 * @param handle simulated value handle
 * @param value the value to set
 */
void HAL_SetSimValue(HAL_SimValueHandle handle, const struct HAL_Value* value);

/**
 * Sets a simulated value (int).
 *
 * @param handle simulated value handle
 * @param value the value to set
 */
inline void HAL_SetSimValueInt(HAL_SimValueHandle handle, int value) {
  struct HAL_Value v = HAL_MakeInt(value);
  HAL_SetSimValue(handle, &v);
}

/**
 * Sets a simulated value (long).
 *
 * @param handle simulated value handle
 * @param value the value to set
 */
inline void HAL_SetSimValueLong(HAL_SimValueHandle handle, int64_t value) {
  struct HAL_Value v = HAL_MakeLong(value);
  HAL_SetSimValue(handle, &v);
}

/**
 * Sets a simulated value (double).
 *
 * @param handle simulated value handle
 * @param value the value to set
 */
inline void HAL_SetSimValueDouble(HAL_SimValueHandle handle, double value) {
  struct HAL_Value v = HAL_MakeDouble(value);
  HAL_SetSimValue(handle, &v);
}

/**
 * Sets a simulated value (enum).
 *
 * @param handle simulated value handle
 * @param value the value to set
 */
inline void HAL_SetSimValueEnum(HAL_SimValueHandle handle, int32_t value) {
  struct HAL_Value v = HAL_MakeEnum(value);
  HAL_SetSimValue(handle, &v);
}

/**
 * Sets a simulated value (boolean).
 *
 * @param handle simulated value handle
 * @param value the value to set
 */
inline void HAL_SetSimValueBoolean(HAL_SimValueHandle handle, HAL_Bool value) {
  struct HAL_Value v = HAL_MakeBoolean(value);
  HAL_SetSimValue(handle, &v);
}

/**
 * Resets a simulated double or integral value to 0.
 * Has no effect on other value types.
 * Use this instead of Set(0) for resetting incremental sensor values like
 * encoder counts or gyro accumulated angle to ensure correct behavior in a
 * distributed system (e.g. WebSockets).
 *
 * @param handle simulated value handle
 */
void HAL_ResetSimValue(HAL_SimValueHandle handle);

/** @} */

#ifdef __cplusplus
}  // extern "C"
#endif

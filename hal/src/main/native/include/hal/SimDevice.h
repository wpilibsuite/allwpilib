// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#ifdef __cplusplus
#include <initializer_list>

#include <wpi/ArrayRef.h>
#endif

#include "hal/Types.h"
#include "hal/Value.h"

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
// clang-format off
HAL_ENUM(HAL_SimValueDirection) {
  HAL_SimValueInput = 0,  /**< input to user code from the simulator */
  HAL_SimValueOutput,     /**< output from user code to the simulator */
  HAL_SimValueBidir       /**< bidirectional between user code and simulator */
};
// clang-format on

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates a simulated device.
 *
 * The device name must be unique.  0 is returned if the device name already
 * exists.  If multiple instances of the same device are desired, recommend
 * appending the instance/unique identifer in brackets to the base name,
 * e.g. "device[1]".
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

#ifdef __cplusplus
extern "C++" {
inline HAL_SimValueHandle HAL_CreateSimValue(HAL_SimDeviceHandle device,
                                             const char* name,
                                             int32_t direction,
                                             const HAL_Value& initialValue) {
  return HAL_CreateSimValue(device, name, direction, &initialValue);
}
}  // extern "C++"
#endif

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

#ifdef __cplusplus
extern "C++" {
inline HAL_Value HAL_GetSimValue(HAL_SimValueHandle handle) {
  HAL_Value v;
  HAL_GetSimValue(handle, &v);
  return v;
}
}  // extern "C++"
#endif

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

#ifdef __cplusplus
extern "C++" {
inline void HAL_SetSimValue(HAL_SimValueHandle handle, const HAL_Value& value) {
  HAL_SetSimValue(handle, &value);
}
}  // extern "C++"
#endif

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

#ifdef __cplusplus
namespace hal {

/**
 * C++ wrapper around a HAL simulator value handle.
 */
class SimValue {
 public:
  /**
   * Default constructor that results in an "empty" object that is false in
   * a boolean context.
   */
  SimValue() = default;

  /**
   * Wraps a simulated value handle as returned by HAL_CreateSimValue().
   *
   * @param handle simulated value handle
   */
  /*implicit*/ SimValue(HAL_SimValueHandle val)  // NOLINT
      : m_handle(val) {}

  /**
   * Determine if handle is empty.  Should be used to optimize out code paths
   * that are taken/not taken in simulation.
   *
   * @return False if handle is empty, true if handle is valid.
   */
  explicit operator bool() const { return m_handle != HAL_kInvalidHandle; }

  /**
   * Get the internal device handle.
   *
   * @return internal handle
   */
  operator HAL_SimValueHandle() const { return m_handle; }  // NOLINT

  /**
   * Gets the simulated value.
   *
   * @return The current value
   */
  HAL_Value GetValue() const { return HAL_GetSimValue(m_handle); }

  /**
   * Sets the simulated value.
   *
   * @param value the value to set
   */
  void SetValue(const HAL_Value& value) { HAL_SetSimValue(m_handle, value); }

 protected:
  HAL_SimValueHandle m_handle = HAL_kInvalidHandle;
};

/**
 * C++ wrapper around a HAL simulator int value handle.
 */
class SimInt : public SimValue {
 public:
  /**
   * Default constructor that results in an "empty" object that is false in
   * a boolean context.
   */
  SimInt() = default;

  /**
   * Wraps a simulated value handle as returned by HAL_CreateSimValueInt().
   *
   * @param handle simulated value handle
   */
  /*implicit*/ SimInt(HAL_SimValueHandle val)  // NOLINT
      : SimValue(val) {}

  /**
   * Gets the simulated value.
   *
   * @return The current value
   */
  int32_t Get() const { return HAL_GetSimValueInt(m_handle); }

  /**
   * Sets the simulated value.
   *
   * @param value the value to set
   */
  void Set(int32_t value) { HAL_SetSimValueInt(m_handle, value); }

  /**
   * Resets the simulated value to 0. Use this instead of Set(0) for resetting
   * incremental sensor values like encoder counts or gyro accumulated angle
   * to ensure correct behavior in a distributed system (e.g. WebSockets).
   */
  void Reset() { HAL_ResetSimValue(m_handle); }
};

/**
 * C++ wrapper around a HAL simulator long value handle.
 */
class SimLong : public SimValue {
 public:
  /**
   * Default constructor that results in an "empty" object that is false in
   * a boolean context.
   */
  SimLong() = default;

  /**
   * Wraps a simulated value handle as returned by HAL_CreateSimValueLong().
   *
   * @param handle simulated value handle
   */
  /*implicit*/ SimLong(HAL_SimValueHandle val)  // NOLINT
      : SimValue(val) {}

  /**
   * Gets the simulated value.
   *
   * @return The current value
   */
  int64_t Get() const { return HAL_GetSimValueLong(m_handle); }

  /**
   * Sets the simulated value.
   *
   * @param value the value to set
   */
  void Set(int64_t value) { HAL_SetSimValueLong(m_handle, value); }

  /**
   * Resets the simulated value to 0. Use this instead of Set(0) for resetting
   * incremental sensor values like encoder counts or gyro accumulated angle
   * to ensure correct behavior in a distributed system (e.g. WebSockets).
   */
  void Reset() { HAL_ResetSimValue(m_handle); }
};

/**
 * C++ wrapper around a HAL simulator double value handle.
 */
class SimDouble : public SimValue {
 public:
  /**
   * Default constructor that results in an "empty" object that is false in
   * a boolean context.
   */
  SimDouble() = default;

  /**
   * Wraps a simulated value handle as returned by HAL_CreateSimValueDouble().
   *
   * @param handle simulated value handle
   */
  /*implicit*/ SimDouble(HAL_SimValueHandle val)  // NOLINT
      : SimValue(val) {}

  /**
   * Gets the simulated value.
   *
   * @return The current value
   */
  double Get() const { return HAL_GetSimValueDouble(m_handle); }

  /**
   * Sets the simulated value.
   *
   * @param value the value to set
   */
  void Set(double value) { HAL_SetSimValueDouble(m_handle, value); }

  /**
   * Resets the simulated value to 0. Use this instead of Set(0) for resetting
   * incremental sensor values like encoder counts or gyro accumulated angle
   * to ensure correct behavior in a distributed system (e.g. WebSockets).
   */
  void Reset() { HAL_ResetSimValue(m_handle); }
};

/**
 * C++ wrapper around a HAL simulator enum value handle.
 */
class SimEnum : public SimValue {
 public:
  /**
   * Default constructor that results in an "empty" object that is false in
   * a boolean context.
   */
  SimEnum() = default;

  /**
   * Wraps a simulated value handle as returned by HAL_CreateSimValueEnum().
   *
   * @param handle simulated value handle
   */
  /*implicit*/ SimEnum(HAL_SimValueHandle val)  // NOLINT
      : SimValue(val) {}

  /**
   * Gets the simulated value.
   *
   * @return The current value
   */
  int32_t Get() const { return HAL_GetSimValueEnum(m_handle); }

  /**
   * Sets the simulated value.
   *
   * @param value the value to set
   */
  void Set(int32_t value) { HAL_SetSimValueEnum(m_handle, value); }
};

/**
 * C++ wrapper around a HAL simulator boolean value handle.
 */
class SimBoolean : public SimValue {
 public:
  /**
   * Default constructor that results in an "empty" object that is false in
   * a boolean context.
   */
  SimBoolean() = default;

  /**
   * Wraps a simulated value handle as returned by HAL_CreateSimValueBoolean().
   *
   * @param handle simulated value handle
   */
  /*implicit*/ SimBoolean(HAL_SimValueHandle val)  // NOLINT
      : SimValue(val) {}

  /**
   * Gets the simulated value.
   *
   * @return The current value
   */
  bool Get() const { return HAL_GetSimValueBoolean(m_handle); }

  /**
   * Sets the simulated value.
   *
   * @param value the value to set
   */
  void Set(bool value) { HAL_SetSimValueBoolean(m_handle, value); }
};

/**
 * A move-only C++ wrapper around a HAL simulator device handle.
 */
class SimDevice {
 public:
  /**
   * Direction of a simulated value (from the perspective of user code).
   */
  enum Direction {
    kInput = HAL_SimValueInput,
    kOutput = HAL_SimValueOutput,
    kBidir = HAL_SimValueBidir
  };

  /**
   * Default constructor that results in an "empty" object that is false in
   * a boolean context.
   */
  SimDevice() = default;

  /**
   * Creates a simulated device.
   *
   * The device name must be unique.  Returns null if the device name
   * already exists.  If multiple instances of the same device are desired,
   * recommend appending the instance/unique identifer in brackets to the base
   * name, e.g. "device[1]".
   *
   * If not in simulation, results in an "empty" object that evaluates to false
   * in a boolean context.
   *
   * @param name device name
   */
  explicit SimDevice(const char* name) : m_handle(HAL_CreateSimDevice(name)) {}

  /**
   * Creates a simulated device.
   *
   * The device name must be unique.  Returns null if the device name
   * already exists.  This is a convenience method that appends index in
   * brackets to the device name, e.g. passing index=1 results in "device[1]"
   * for the device name.
   *
   * If not in simulation, results in an "empty" object that evaluates to false
   * in a boolean context.
   *
   * @param name device name
   * @param index device index number to append to name
   */
  SimDevice(const char* name, int index);

  /**
   * Creates a simulated device.
   *
   * The device name must be unique.  Returns null if the device name
   * already exists.  This is a convenience method that appends index and
   * channel in brackets to the device name, e.g. passing index=1 and channel=2
   * results in "device[1,2]" for the device name.
   *
   * If not in simulation, results in an "empty" object that evaluates to false
   * in a boolean context.
   *
   * @param name device name
   * @param index device index number to append to name
   * @param channel device channel number to append to name
   */
  SimDevice(const char* name, int index, int channel);

  ~SimDevice() {
    if (m_handle != HAL_kInvalidHandle) {
      HAL_FreeSimDevice(m_handle);
    }
  }

  SimDevice(const SimDevice&) = delete;
  SimDevice& operator=(const SimDevice&) = delete;

  SimDevice(SimDevice&& rhs) : m_handle(rhs.m_handle) {
    rhs.m_handle = HAL_kInvalidHandle;
  }

  SimDevice& operator=(SimDevice&& rhs) {
    m_handle = rhs.m_handle;
    rhs.m_handle = HAL_kInvalidHandle;
    return *this;
  }

  /**
   * Determine if handle is empty.  Should be used to optimize out code paths
   * that are taken/not taken in simulation.
   *
   * @return False if handle is empty, true if handle is valid.
   */
  explicit operator bool() const { return m_handle != HAL_kInvalidHandle; }

  /**
   * Get the internal device handle.
   *
   * @return internal handle
   */
  operator HAL_SimDeviceHandle() const { return m_handle; }  // NOLINT

  /**
   * Creates a value on the simulated device.
   *
   * If not in simulation, results in an "empty" object that evaluates to false
   * in a boolean context.
   *
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param initialValue initial value
   * @return simulated value object
   */
  SimValue CreateValue(const char* name, int32_t direction,
                       const HAL_Value& initialValue) {
    return HAL_CreateSimValue(m_handle, name, direction, &initialValue);
  }

  /**
   * Creates a double value on the simulated device.
   *
   * If not in simulation, results in an "empty" object that evaluates to false
   * in a boolean context.
   *
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param initialValue initial value
   * @return simulated double value object
   */
  SimDouble CreateDouble(const char* name, int32_t direction,
                         double initialValue) {
    return HAL_CreateSimValueDouble(m_handle, name, direction, initialValue);
  }

  /**
   * Creates an enumerated value on the simulated device.
   *
   * Enumerated values are always in the range 0 to numOptions-1.
   *
   * If not in simulation, results in an "empty" object that evaluates to false
   * in a boolean context.
   *
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param options array of option descriptions
   * @param initialValue initial value (selection)
   * @return simulated enum value object
   */
  SimEnum CreateEnum(const char* name, int32_t direction,
                     std::initializer_list<const char*> options,
                     int32_t initialValue) {
    return HAL_CreateSimValueEnum(m_handle, name, direction, options.size(),
                                  const_cast<const char**>(options.begin()),
                                  initialValue);
  }

  /**
   * Creates an enumerated value on the simulated device.
   *
   * Enumerated values are always in the range 0 to numOptions-1.
   *
   * If not in simulation, results in an "empty" object that evaluates to false
   * in a boolean context.
   *
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param options array of option descriptions
   * @param initialValue initial value (selection)
   * @return simulated enum value object
   */
  SimEnum CreateEnum(const char* name, int32_t direction,
                     wpi::ArrayRef<const char*> options, int32_t initialValue) {
    return HAL_CreateSimValueEnum(m_handle, name, direction, options.size(),
                                  const_cast<const char**>(options.data()),
                                  initialValue);
  }

  /**
   * Creates an enumerated value on the simulated device with double values.
   *
   * Enumerated values are always in the range 0 to numOptions-1.
   *
   * If not in simulation, results in an "empty" object that evaluates to false
   * in a boolean context.
   *
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param options array of option descriptions
   * @param optionValues array of option values (must be the same size as
   *                     options)
   * @param initialValue initial value (selection)
   * @return simulated enum value object
   */
  SimEnum CreateEnumDouble(const char* name, int32_t direction,
                           std::initializer_list<const char*> options,
                           std::initializer_list<double> optionValues,
                           int32_t initialValue) {
    if (options.size() != optionValues.size()) {
      return {};
    }
    return HAL_CreateSimValueEnumDouble(
        m_handle, name, direction, options.size(),
        const_cast<const char**>(options.begin()), optionValues.begin(),
        initialValue);
  }

  /**
   * Creates an enumerated value on the simulated device with double values.
   *
   * Enumerated values are always in the range 0 to numOptions-1.
   *
   * If not in simulation, results in an "empty" object that evaluates to false
   * in a boolean context.
   *
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param options array of option descriptions
   * @param optionValues array of option values (must be the same size as
   *                     options)
   * @param initialValue initial value (selection)
   * @return simulated enum value object
   */
  SimEnum CreateEnumDouble(const char* name, int32_t direction,
                           wpi::ArrayRef<const char*> options,
                           wpi::ArrayRef<double> optionValues,
                           int32_t initialValue) {
    if (options.size() != optionValues.size()) {
      return {};
    }
    return HAL_CreateSimValueEnumDouble(
        m_handle, name, direction, options.size(),
        const_cast<const char**>(options.data()), optionValues.data(),
        initialValue);
  }

  /**
   * Creates a boolean value on the simulated device.
   *
   * If not in simulation, results in an "empty" object that evaluates to false
   * in a boolean context.
   *
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param initialValue initial value
   * @return simulated boolean value object
   */
  SimBoolean CreateBoolean(const char* name, int32_t direction,
                           bool initialValue) {
    return HAL_CreateSimValueBoolean(m_handle, name, direction, initialValue);
  }

 protected:
  HAL_SimDeviceHandle m_handle = HAL_kInvalidHandle;
};

}  // namespace hal
#endif  // __cplusplus

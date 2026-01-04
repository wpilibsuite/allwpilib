// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <initializer_list>
#include <span>
#include <string>

#include "wpi/hal/SimDevice.h"

inline HAL_SimValueHandle HAL_CreateSimValue(HAL_SimDeviceHandle device,
                                             const char* name,
                                             int32_t direction,
                                             const HAL_Value& initialValue) {
  return HAL_CreateSimValue(device, name, direction, &initialValue);
}

inline HAL_Value HAL_GetSimValue(HAL_SimValueHandle handle) {
  HAL_Value v;
  HAL_GetSimValue(handle, &v);
  return v;
}

inline void HAL_SetSimValue(HAL_SimValueHandle handle, const HAL_Value& value) {
  HAL_SetSimValue(handle, &value);
}

namespace wpi::hal {

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
   * @param val simulated value handle
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
   * @param val simulated value handle
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
   * @param val simulated value handle
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
   * @param val simulated value handle
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
   * @param val simulated value handle
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
   * @param val simulated value handle
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
   * recommend appending the instance/unique identifier in brackets to the base
   * name, e.g. "device[1]".
   *
   * Using a device name of the form "Type:Name" will create a WebSockets node
   * with a type value of "Type" and a device value of "Name"
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
   * Using a device name of the form "Type:Name" will create a WebSockets node
   * with a type value of "Type" and a device value of "Name"
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
   * Using a device name of the form "Type:Name" will create a WebSockets node
   * with a type value of "Type" and a device value of "Name"
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
   * Get the name of the simulated device.
   *
   * @return name
   */
  std::string GetName() const {
    return std::string(HAL_GetSimDeviceName(m_handle));
  }

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
   * Creates an int value on the simulated device.
   *
   * If not in simulation, results in an "empty" object that evaluates to false
   * in a boolean context.
   *
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param initialValue initial value
   * @return simulated double value object
   */
  SimInt CreateInt(const char* name, int32_t direction, int32_t initialValue) {
    return HAL_CreateSimValueInt(m_handle, name, direction, initialValue);
  }

  /**
   * Creates a long value on the simulated device.
   *
   * If not in simulation, results in an "empty" object that evaluates to false
   * in a boolean context.
   *
   * @param name value name
   * @param direction input/output/bidir (from perspective of user code)
   * @param initialValue initial value
   * @return simulated double value object
   */
  SimLong CreateLong(const char* name, int32_t direction,
                     int64_t initialValue) {
    return HAL_CreateSimValueLong(m_handle, name, direction, initialValue);
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
                     std::span<const char* const> options,
                     int32_t initialValue) {
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
                           std::span<const char* const> options,
                           std::span<const double> optionValues,
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

}  // namespace wpi::hal

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <string>
#include <vector>

#include <hal/SimDevice.h>
#include <hal/simulation/SimDeviceData.h>

namespace frc::sim {

/**
 * Class to control the simulation side of a SimDevice.
 */
class SimDeviceSim {
 public:
  /**
   * Constructs a SimDeviceSim.
   *
   * @param name name of the SimDevice
   */
  explicit SimDeviceSim(const char* name);

  /**
   * Get the property object with the given name.
   *
   * @param name the property name
   * @return the property object
   */
  hal::SimValue GetValue(const char* name) const;

  /**
   * Get the property object with the given name.
   *
   * @param name the property name
   * @return the property object
   */
  hal::SimInt GetInt(const char* name) const;

  /**
   * Get the property object with the given name.
   *
   * @param name the property name
   * @return the property object
   */
  hal::SimLong GetLong(const char* name) const;

  /**
   * Get the property object with the given name.
   *
   * @param name the property name
   * @return the property object
   */
  hal::SimDouble GetDouble(const char* name) const;

  /**
   * Get the property object with the given name.
   *
   * @param name the property name
   * @return the property object
   */
  hal::SimEnum GetEnum(const char* name) const;

  /**
   * Get the property object with the given name.
   *
   * @param name the property name
   * @return the property object
   */
  hal::SimBoolean GetBoolean(const char* name) const;

  /**
   * Get all options for the given enum.
   *
   * @param val the enum
   * @return names of the different values for that enum
   */
  static std::vector<std::string> GetEnumOptions(hal::SimEnum val);

  /**
   * Get all properties.
   *
   * @param callback callback called for each property (SimValue).  Signature
   *                 of the callback must be const char*, HAL_SimValueHandle,
   *                 int, const HAL_Value*
   */
  template <typename F>
  void EnumerateValues(F callback) const {
    return HALSIM_EnumerateSimValues(
        m_handle, &callback,
        [](const char* name, void* param, HAL_SimValueHandle handle,
           int direction, const struct HAL_Value* value) {
          std::invoke(*static_cast<F*>(param), name, handle, direction, value);
        });
  }

  /**
   * Get the raw handle of this object.
   *
   * @return the handle used to refer to this object
   */
  operator HAL_SimDeviceHandle() const { return m_handle; }  // NOLINT

  /**
   * Get all sim devices with the given prefix.
   *
   * @param prefix the prefix to filter sim devices
   * @param callback callback function to call for each sim device
   */
  template <typename F>
  static void EnumerateDevices(const char* prefix, F callback) {
    return HALSIM_EnumerateSimDevices(
        prefix, &callback,
        [](const char* name, void* param, HAL_SimDeviceHandle handle) {
          std::invoke(*static_cast<F*>(param), name, handle);
        });
  }

  /**
   * Reset all SimDevice data.
   */
  static void ResetData();

 private:
  HAL_SimDeviceHandle m_handle;
};
}  // namespace frc::sim

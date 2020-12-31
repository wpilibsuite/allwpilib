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

  hal::SimValue GetValue(const char* name) const;

  hal::SimDouble GetDouble(const char* name) const;

  hal::SimEnum GetEnum(const char* name) const;

  hal::SimBoolean GetBoolean(const char* name) const;

  static std::vector<std::string> GetEnumOptions(hal::SimEnum val);

  template <typename F>
  void EnumerateValues(F callback) const {
    return HALSIM_EnumerateSimValues(
        m_handle, &callback,
        [](const char* name, void* param, HAL_SimValueHandle handle,
           HAL_Bool readonly, const struct HAL_Value* value) {
          std::invoke(*static_cast<F*>(param), name, handle, readonly, value);
        });
  }

  operator HAL_SimDeviceHandle() const { return m_handle; }  // NOLINT

  template <typename F>
  static void EnumerateDevices(const char* prefix, F callback) {
    return HALSIM_EnumerateSimDevices(
        prefix, &callback,
        [](const char* name, void* param, HAL_SimDeviceHandle handle) {
          std::invoke(*static_cast<F*>(param), name, handle);
        });
  }

  static void ResetData();

 private:
  HAL_SimDeviceHandle m_handle;
};
}  // namespace frc::sim

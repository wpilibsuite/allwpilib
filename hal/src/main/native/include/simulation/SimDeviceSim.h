/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "CallbackStore.h"
#include "hal/SimDevice.h"
#include "mockdata/SimDeviceData.h"

namespace frc {
namespace sim {
class SimDeviceSim {
 public:
  explicit SimDeviceSim(const char* name)
      : m_handle{HALSIM_GetSimDeviceHandle(name)} {}

  hal::SimValue GetValue(const char* name) const {
    return HALSIM_GetSimValueHandle(m_handle, name);
  }

  hal::SimDouble GetDouble(const char* name) const {
    return HALSIM_GetSimValueHandle(m_handle, name);
  }

  hal::SimEnum GetEnum(const char* name) const {
    return HALSIM_GetSimValueHandle(m_handle, name);
  }

  hal::SimBoolean GetBoolean(const char* name) const {
    return HALSIM_GetSimValueHandle(m_handle, name);
  }

  static std::vector<std::string> GetEnumOptions(hal::SimEnum val) {
    int32_t numOptions;
    const char** options = HALSIM_GetSimValueEnumOptions(val, &numOptions);
    std::vector<std::string> rv;
    rv.reserve(numOptions);
    for (int32_t i = 0; i < numOptions; ++i) rv.emplace_back(options[i]);
    return rv;
  }

  template <typename F>
  void EnumerateValues(F callback) const {
    return HALSIM_EnumerateSimValues(
        m_handle, &callback,
        [](const char* name, void* param, HAL_SimValueHandle handle,
           HAL_Bool readonly, const struct HAL_Value* value) {
          std::invoke(*static_cast<F*>(param), name, handle, readonly, value);
        });
  }

  operator HAL_SimDeviceHandle() const { return m_handle; }

  template <typename F>
  static void EnumerateDevices(const char* prefix, F callback) {
    return HALSIM_EnumerateSimDevices(
        prefix, &callback,
        [](const char* name, void* param, HAL_SimDeviceHandle handle) {
          std::invoke(*static_cast<F*>(param), name, handle);
        });
  }

  static void ResetData() { HALSIM_ResetSimDeviceData(); }

 private:
  HAL_SimDeviceHandle m_handle;
};
}  // namespace sim
}  // namespace frc

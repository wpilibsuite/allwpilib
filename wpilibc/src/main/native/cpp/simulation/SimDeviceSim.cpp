// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/SimDeviceSim.h"

#include <string>
#include <vector>

#include <hal/SimDevice.h>
#include <hal/simulation/SimDeviceData.h>

using namespace frc;
using namespace frc::sim;

SimDeviceSim::SimDeviceSim(const char* name)
    : m_handle{HALSIM_GetSimDeviceHandle(name)} {}

hal::SimValue SimDeviceSim::GetValue(const char* name) const {
  return HALSIM_GetSimValueHandle(m_handle, name);
}

hal::SimInt SimDeviceSim::GetInt(const char* name) const {
  return HALSIM_GetSimValueHandle(m_handle, name);
}

hal::SimLong SimDeviceSim::GetLong(const char* name) const {
  return HALSIM_GetSimValueHandle(m_handle, name);
}

hal::SimDouble SimDeviceSim::GetDouble(const char* name) const {
  return HALSIM_GetSimValueHandle(m_handle, name);
}

hal::SimEnum SimDeviceSim::GetEnum(const char* name) const {
  return HALSIM_GetSimValueHandle(m_handle, name);
}

hal::SimBoolean SimDeviceSim::GetBoolean(const char* name) const {
  return HALSIM_GetSimValueHandle(m_handle, name);
}

std::vector<std::string> SimDeviceSim::GetEnumOptions(hal::SimEnum val) {
  int32_t numOptions;
  const char** options = HALSIM_GetSimValueEnumOptions(val, &numOptions);
  std::vector<std::string> rv;
  rv.reserve(numOptions);
  for (int32_t i = 0; i < numOptions; ++i) {
    rv.emplace_back(options[i]);
  }
  return rv;
}

void SimDeviceSim::ResetData() {
  HALSIM_ResetSimDeviceData();
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/simulation/SimDeviceSim.h"

#include <string>
#include <vector>

#include <hal/SimDevice.h>
#include <hal/simulation/SimDeviceData.h>

using namespace frc;
using namespace frc::sim;

SimDeviceSim::SimDeviceSim(const char* name)
    : m_handle{HALSIM_GetSimDeviceHandle(name)} {}

const char* SimDeviceSim::GetDisplayName() const {
  return HALSIM_GetSimDeviceDisplayName(m_handle);
}
void SimDeviceSim::SetDisplayName(const char* displayName) {
  HALSIM_SetSimDeviceDisplayName(m_handle, displayName);
}

hal::SimValue SimDeviceSim::GetValue(const char* name) const {
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
  for (int32_t i = 0; i < numOptions; ++i) rv.emplace_back(options[i]);
  return rv;
}

void SimDeviceSim::ResetData() { HALSIM_ResetSimDeviceData(); }

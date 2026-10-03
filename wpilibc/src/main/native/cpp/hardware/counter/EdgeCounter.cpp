// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/counter/EdgeCounter.hpp"

#include <string>

#include "wpi/hal/Counter.h"
#include "wpi/system/Errors.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpi/util/StackTrace.hpp"
#include "wpi/util/UsageReporting.hpp"

using namespace wpi;

EdgeCounter::EdgeCounter(int channel, EdgeConfiguration configuration)
    : m_channel{channel} {
  int32_t status = 0;
  std::string stackTrace = wpi::util::GetStackTrace(1);
  m_handle = HAL_InitializeCounter(
      channel, configuration == EdgeConfiguration::RISING_EDGE,
      stackTrace.c_str(), &status);
  WPILIB_CheckErrorStatus(status, "{}", channel);

  Reset();

  wpi::util::ReportUsage("IO", channel, "EdgeCounter");
}

int EdgeCounter::GetCount() const {
  int32_t status = 0;
  int val = HAL_GetCounter(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "{}", m_channel);
  return val;
}

void EdgeCounter::Reset() {
  int32_t status = 0;
  HAL_ResetCounter(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "{}", m_channel);
}

void EdgeCounter::SetEdgeConfiguration(EdgeConfiguration configuration) {
  int32_t status = 0;
  bool rising = configuration == EdgeConfiguration::RISING_EDGE;
  HAL_SetCounterEdgeConfiguration(m_handle, rising, &status);
  WPILIB_CheckErrorStatus(status, "{}", m_channel);
}

void EdgeCounter::LogTo(wpi::telemetry::TelemetryTable& table) const {
  table.Log("Count", GetCount());
}

std::string_view EdgeCounter::GetTelemetryType() const {
  return "Edge Counter";
}

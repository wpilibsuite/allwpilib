// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/counter/UpDownCounter.hpp"

#include <string>

#include "wpi/hal/Counter.h"
#include "wpi/hal/UsageReporting.h"
#include "wpi/system/Errors.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpi/util/StackTrace.hpp"

using namespace wpi;

UpDownCounter::UpDownCounter(int channel, EdgeConfiguration configuration)
    : m_channel{channel} {
  int32_t status = 0;
  std::string stackTrace = wpi::util::GetStackTrace(1);
  m_handle = HAL_InitializeCounter(
      channel, configuration == EdgeConfiguration::kRisingEdge,
      stackTrace.c_str(), &status);
  WPILIB_CheckErrorStatus(status, "{}", channel);

  Reset();

  HAL_ReportUsage("IO", channel, "UpDownCounter");
}

int UpDownCounter::GetCount() const {
  int32_t status = 0;
  int val = HAL_GetCounter(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "{}", m_channel);
  return val;
}

void UpDownCounter::Reset() {
  int32_t status = 0;
  HAL_ResetCounter(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "{}", m_channel);
}

void UpDownCounter::SetEdgeConfiguration(EdgeConfiguration configuration) {
  int32_t status = 0;
  bool rising = configuration == EdgeConfiguration::kRisingEdge;
  HAL_SetCounterEdgeConfiguration(m_handle, rising, &status);
  WPILIB_CheckErrorStatus(status, "{}", m_channel);
}

void UpDownCounter::UpdateTelemetry(wpi::TelemetryTable& table) const {
  table.Log("Count", GetCount());
}

std::string_view UpDownCounter::GetTelemetryType() const {
  return "UpDown Counter";
}

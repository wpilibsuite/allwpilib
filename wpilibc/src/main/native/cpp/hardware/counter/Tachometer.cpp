// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/counter/Tachometer.hpp"

#include <string>

#include "wpi/hal/Counter.h"
#include "wpi/system/Errors.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpi/util/StackTrace.hpp"
#include "wpi/util/UsageReporting.hpp"

using namespace wpi;

Tachometer::Tachometer(int channel, EdgeConfiguration configuration)
    : m_channel{channel} {
  int32_t status = 0;
  std::string stackTrace = wpi::util::GetStackTrace(1);
  m_handle = HAL_InitializeCounter(
      channel, configuration == EdgeConfiguration::RISING_EDGE,
      stackTrace.c_str(), &status);
  WPILIB_CheckErrorStatus(status, "{}", channel);

  wpi::util::ReportUsage("IO", channel, "Tachometer");
}

void Tachometer::SetEdgeConfiguration(EdgeConfiguration configuration) {
  int32_t status = 0;
  bool rising = configuration == EdgeConfiguration::RISING_EDGE;
  HAL_SetCounterEdgeConfiguration(m_handle, rising, &status);
  WPILIB_CheckErrorStatus(status, "{}", m_channel);
}

void Tachometer::SetRateWindow(wpi::units::milliseconds<> window) {
  int32_t status = 0;
  HAL_SetCounterRateWindow(m_handle, static_cast<int32_t>(window.value()),
                           &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);
}

wpi::units::hertz<> Tachometer::GetFrequency() const {
  int32_t status = 0;
  double rate = HAL_GetCounterRate(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);
  return wpi::units::hertz<>{rate};
}

int Tachometer::GetEdgesPerRevolution() const {
  return m_edgesPerRevolution;
}
void Tachometer::SetEdgesPerRevolution(int edges) {
  m_edgesPerRevolution = edges;
}

wpi::units::turns_per_second<> Tachometer::GetRevolutionsPerSecond() const {
  int edgesPerRevolution = GetEdgesPerRevolution();
  if (edgesPerRevolution == 0) {
    return 0_tps;
  }
  auto rotationHz = GetFrequency() / edgesPerRevolution;
  return wpi::units::turns_per_second<>{rotationHz.value()};
}

wpi::units::revolutions_per_minute<> Tachometer::GetRevolutionsPerMinute()
    const {
  return wpi::units::revolutions_per_minute<>{GetRevolutionsPerSecond()};
}

bool Tachometer::GetStopped() const {
  int32_t status = 0;
  bool stopped = HAL_GetCounterStopped(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);
  return stopped;
}

void Tachometer::LogTo(wpi::telemetry::TelemetryTable& table) const {
  table.Log("RPM", GetRevolutionsPerMinute());
}

std::string_view Tachometer::GetTelemetryType() const {
  return "Tachometer";
}

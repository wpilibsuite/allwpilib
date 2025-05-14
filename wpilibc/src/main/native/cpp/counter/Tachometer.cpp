// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/counter/Tachometer.h"

#include <string>

#include <hal/Counter.h>
#include <hal/UsageReporting.h>
#include <wpi/StackTrace.h>
#include <wpi/sendable/SendableBuilder.h>

#include "frc/Errors.h"

using namespace frc;

Tachometer::Tachometer(int channel, EdgeConfiguration configuration)
    : m_channel{channel} {
  int32_t status = 0;
  std::string stackTrace = wpi::GetStackTrace(1);
  m_handle = HAL_InitializeCounter(
      channel, configuration == EdgeConfiguration::RISING_EDGE,
      stackTrace.c_str(), &status);
  FRC_CheckErrorStatus(status, "{}", channel);

  HAL_ReportUsage("IO", channel, "Tachometer");
  wpi::SendableRegistry::Add(this, "Tachometer", channel);
}

void Tachometer::SetEdgeConfiguration(EdgeConfiguration configuration) {
  int32_t status = 0;
  bool rising = configuration == EdgeConfiguration::RISING_EDGE;
  HAL_SetCounterEdgeConfiguration(m_handle, rising, &status);
  FRC_CheckErrorStatus(status, "{}", m_channel);
}

units::hertz_t Tachometer::GetFrequency() const {
  auto period = GetPeriod();
  if (period.value() == 0) {
    return 0_Hz;
  }
  return 1 / period;
}

units::second_t Tachometer::GetPeriod() const {
  int32_t status = 0;
  double period = HAL_GetCounterPeriod(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return units::second_t{period};
}

int Tachometer::GetEdgesPerRevolution() const {
  return m_edgesPerRevolution;
}
void Tachometer::SetEdgesPerRevolution(int edges) {
  m_edgesPerRevolution = edges;
}

units::turns_per_second_t Tachometer::GetRevolutionsPerSecond() const {
  auto period = GetPeriod();
  if (period.value() == 0) {
    return 0_tps;
  }
  int edgesPerRevolution = GetEdgesPerRevolution();
  if (edgesPerRevolution == 0) {
    return 0_tps;
  }
  auto rotationHz = ((1.0 / edgesPerRevolution) / period);
  return units::turns_per_second_t{rotationHz.value()};
}

units::revolutions_per_minute_t Tachometer::GetRevolutionsPerMinute() const {
  return units::revolutions_per_minute_t{GetRevolutionsPerSecond()};
}

bool Tachometer::GetStopped() const {
  int32_t status = 0;
  bool stopped = HAL_GetCounterStopped(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return stopped;
}

void Tachometer::SetMaxPeriod(units::second_t maxPeriod) {
  int32_t status = 0;
  HAL_SetCounterMaxPeriod(m_handle, maxPeriod.value(), &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

void Tachometer::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Tachometer");
  builder.AddDoubleProperty(
      "RPS", [&] { return GetRevolutionsPerSecond().value(); }, nullptr);
  builder.AddDoubleProperty(
      "RPM", [&] { return GetRevolutionsPerMinute().value(); }, nullptr);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/counter/UpDownCounter.h"

#include <memory>
#include <string>

#include <hal/Counter.h>
#include <hal/UsageReporting.h>
#include <wpi/StackTrace.h>
#include <wpi/sendable/SendableBuilder.h>

#include "frc/Errors.h"

using namespace frc;

UpDownCounter::UpDownCounter(int channel, EdgeConfiguration configuration)
    : m_channel{channel} {
  int32_t status = 0;
  std::string stackTrace = wpi::GetStackTrace(1);
  m_handle = HAL_InitializeCounter(
      channel, configuration == EdgeConfiguration::RISING_EDGE,
      stackTrace.c_str(), &status);
  FRC_CheckErrorStatus(status, "{}", channel);

  Reset();

  HAL_ReportUsage("IO", channel, "UpDownCounter");
  wpi::SendableRegistry::Add(this, "UpDown Counter", channel);
}

int UpDownCounter::GetCount() const {
  int32_t status = 0;
  int val = HAL_GetCounter(m_handle, &status);
  FRC_CheckErrorStatus(status, "{}", m_channel);
  return val;
}

void UpDownCounter::Reset() {
  int32_t status = 0;
  HAL_ResetCounter(m_handle, &status);
  FRC_CheckErrorStatus(status, "{}", m_channel);
}

void UpDownCounter::SetEdgeConfiguration(EdgeConfiguration configuration) {
  int32_t status = 0;
  bool rising = configuration == EdgeConfiguration::RISING_EDGE;
  HAL_SetCounterEdgeConfiguration(m_handle, rising, &status);
  FRC_CheckErrorStatus(status, "{}", m_channel);
}

void UpDownCounter::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("UpDown Counter");
  builder.AddDoubleProperty("Count", [&] { return GetCount(); }, nullptr);
}

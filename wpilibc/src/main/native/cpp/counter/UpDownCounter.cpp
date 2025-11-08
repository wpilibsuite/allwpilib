// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/counter/UpDownCounter.hpp"

#include <memory>
#include <string>

#include "wpi/hal/Counter.h"
#include "wpi/hal/UsageReporting.h"
#include "wpi/system/Errors.hpp"
#include "wpi/util/StackTrace.hpp"
#include "wpi/util/sendable/SendableBuilder.hpp"

using namespace wpi;

UpDownCounter::UpDownCounter(int channel, EdgeConfiguration configuration)
    : m_channel{channel} {
  int32_t status = 0;
  std::string stackTrace = wpi::util::GetStackTrace(1);
  m_handle = HAL_InitializeCounter(
      channel, configuration == EdgeConfiguration::kRisingEdge,
      stackTrace.c_str(), &status);
  FRC_CheckErrorStatus(status, "{}", channel);

  Reset();

  HAL_ReportUsage("IO", channel, "UpDownCounter");
  wpi::util::SendableRegistry::Add(this, "UpDown Counter", channel);
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
  bool rising = configuration == EdgeConfiguration::kRisingEdge;
  HAL_SetCounterEdgeConfiguration(m_handle, rising, &status);
  FRC_CheckErrorStatus(status, "{}", m_channel);
}

void UpDownCounter::InitSendable(wpi::util::SendableBuilder& builder) {
  builder.SetSmartDashboardType("UpDown Counter");
  builder.AddDoubleProperty("Count", [&] { return GetCount(); }, nullptr);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/counter/UpDownCounter.h"

#include <memory>

#include <hal/Counter.h>
#include <hal/FRCUsageReporting.h>
#include <wpi/NullDeleter.h>
#include <wpi/sendable/SendableBuilder.h>

#include "frc/DigitalSource.h"
#include "frc/Errors.h"

using namespace frc;

UpDownCounter::UpDownCounter(DigitalSource& upSource, DigitalSource& downSource)
    : UpDownCounter({&upSource, wpi::NullDeleter<DigitalSource>()},
                    {&downSource, wpi::NullDeleter<DigitalSource>()}) {}

UpDownCounter::UpDownCounter(std::shared_ptr<DigitalSource> upSource,
                             std::shared_ptr<DigitalSource> downSource) {
  m_upSource = upSource;
  m_downSource = downSource;

  int32_t status = 0;
  m_handle = HAL_InitializeCounter(HAL_Counter_Mode::HAL_Counter_kTwoPulse,
                                   &m_index, &status);
  FRC_CheckErrorStatus(status, "{}", m_index);

  if (m_upSource) {
    HAL_SetCounterUpSource(m_handle, m_upSource->GetPortHandleForRouting(),
                           static_cast<HAL_AnalogTriggerType>(
                               m_upSource->GetAnalogTriggerTypeForRouting()),
                           &status);
    FRC_CheckErrorStatus(status, "{}", m_index);
    HAL_SetCounterUpSourceEdge(m_handle, true, false, &status);
    FRC_CheckErrorStatus(status, "{}", m_index);
  }

  if (m_downSource) {
    HAL_SetCounterDownSource(
        m_handle, m_downSource->GetPortHandleForRouting(),
        static_cast<HAL_AnalogTriggerType>(
            m_downSource->GetAnalogTriggerTypeForRouting()),
        &status);
    FRC_CheckErrorStatus(status, "{}", m_index);
    HAL_SetCounterDownSourceEdge(m_handle, true, false, &status);
    FRC_CheckErrorStatus(status, "{}", m_index);
  }

  Reset();

  HAL_Report(HALUsageReporting::kResourceType_Counter, m_index + 1);
  wpi::SendableRegistry::AddLW(this, "UpDown Counter", m_index);
}

int UpDownCounter::GetCount() const {
  int32_t status = 0;
  int val = HAL_GetCounter(m_handle, &status);
  FRC_CheckErrorStatus(status, "{}", m_index);
  return val;
}

void UpDownCounter::SetReverseDirection(bool reverseDirection) {
  int32_t status = 0;
  HAL_SetCounterReverseDirection(m_handle, reverseDirection, &status);
  FRC_CheckErrorStatus(status, "{}", m_index);
}

void UpDownCounter::Reset() {
  int32_t status = 0;
  HAL_ResetCounter(m_handle, &status);
  FRC_CheckErrorStatus(status, "{}", m_index);
}

void UpDownCounter::SetUpEdgeConfiguration(EdgeConfiguration configuration) {
  int32_t status = 0;
  bool rising = configuration == EdgeConfiguration::kRisingEdge ||
                configuration == EdgeConfiguration::kBoth;
  bool falling = configuration == EdgeConfiguration::kFallingEdge ||
                 configuration == EdgeConfiguration::kBoth;
  HAL_SetCounterUpSourceEdge(m_handle, rising, falling, &status);
  FRC_CheckErrorStatus(status, "{}", m_index);
}

void UpDownCounter::SetDownEdgeConfiguration(EdgeConfiguration configuration) {
  int32_t status = 0;
  bool rising = configuration == EdgeConfiguration::kRisingEdge ||
                configuration == EdgeConfiguration::kBoth;
  bool falling = configuration == EdgeConfiguration::kFallingEdge ||
                 configuration == EdgeConfiguration::kBoth;
  HAL_SetCounterDownSourceEdge(m_handle, rising, falling, &status);
  FRC_CheckErrorStatus(status, "{}", m_index);
}

void UpDownCounter::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("UpDown Counter");
  builder.AddDoubleProperty("Count", [&] { return GetCount(); }, nullptr);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/counter/ExternalDirectionCounter.h"

#include <memory>

#include <hal/Counter.h>
#include <hal/FRCUsageReporting.h>
#include <wpi/NullDeleter.h>
#include <wpi/sendable/SendableBuilder.h>

#include "frc/DigitalSource.h"
#include "frc/Errors.h"

using namespace frc;

ExternalDirectionCounter::ExternalDirectionCounter(
    DigitalSource& countSource, DigitalSource& directionSource)
    : ExternalDirectionCounter(
          {&countSource, wpi::NullDeleter<DigitalSource>()},
          {&directionSource, wpi::NullDeleter<DigitalSource>()}) {}

ExternalDirectionCounter::ExternalDirectionCounter(
    std::shared_ptr<DigitalSource> countSource,
    std::shared_ptr<DigitalSource> directionSource) {
  if (countSource == nullptr) {
    throw FRC_MakeError(err::NullParameter, "countSource");
  }
  if (directionSource == nullptr) {
    throw FRC_MakeError(err::NullParameter, "directionSource");
  }

  m_countSource = countSource;
  m_directionSource = directionSource;

  int32_t status = 0;
  m_handle = HAL_InitializeCounter(
      HAL_Counter_Mode::HAL_Counter_kExternalDirection, &m_index, &status);
  FRC_CheckErrorStatus(status, "{}", m_index);

  HAL_SetCounterUpSource(m_handle, m_countSource->GetPortHandleForRouting(),
                         static_cast<HAL_AnalogTriggerType>(
                             m_countSource->GetAnalogTriggerTypeForRouting()),
                         &status);
  FRC_CheckErrorStatus(status, "{}", m_index);
  HAL_SetCounterUpSourceEdge(m_handle, true, false, &status);
  FRC_CheckErrorStatus(status, "{}", m_index);

  HAL_SetCounterDownSource(
      m_handle, m_directionSource->GetPortHandleForRouting(),
      static_cast<HAL_AnalogTriggerType>(
          m_directionSource->GetAnalogTriggerTypeForRouting()),
      &status);
  FRC_CheckErrorStatus(status, "{}", m_index);
  HAL_SetCounterDownSourceEdge(m_handle, false, true, &status);
  FRC_CheckErrorStatus(status, "{}", m_index);

  Reset();

  HAL_Report(HALUsageReporting::kResourceType_Counter, m_index + 1);
  wpi::SendableRegistry::AddLW(this, "External Direction Counter", m_index);
}

int ExternalDirectionCounter::GetCount() const {
  int32_t status = 0;
  int val = HAL_GetCounter(m_handle, &status);
  FRC_CheckErrorStatus(status, "{}", m_index);
  return val;
}

void ExternalDirectionCounter::SetReverseDirection(bool reverseDirection) {
  int32_t status = 0;
  HAL_SetCounterReverseDirection(m_handle, reverseDirection, &status);
  FRC_CheckErrorStatus(status, "{}", m_index);
}

void ExternalDirectionCounter::Reset() {
  int32_t status = 0;
  HAL_ResetCounter(m_handle, &status);
  FRC_CheckErrorStatus(status, "{}", m_index);
}

void ExternalDirectionCounter::SetEdgeConfiguration(
    EdgeConfiguration configuration) {
  int32_t status = 0;
  bool rising = configuration == EdgeConfiguration::kRisingEdge ||
                configuration == EdgeConfiguration::kBoth;
  bool falling = configuration == EdgeConfiguration::kFallingEdge ||
                 configuration == EdgeConfiguration::kBoth;
  HAL_SetCounterUpSourceEdge(m_handle, rising, falling, &status);
  FRC_CheckErrorStatus(status, "{}", m_index);
}

void ExternalDirectionCounter::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("External Direction Counter");
  builder.AddDoubleProperty("Count", [&] { return GetCount(); }, nullptr);
}

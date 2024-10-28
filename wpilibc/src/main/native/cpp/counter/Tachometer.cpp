// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/counter/Tachometer.h"

#include <frc/DigitalSource.h>

#include <hal/Counter.h>
#include <hal/FRCUsageReporting.h>
#include <wpi/NullDeleter.h>
#include <wpi/sendable/SendableBuilder.h>

#include "frc/Errors.h"

using namespace frc;

Tachometer::Tachometer(DigitalSource& source)
    : Tachometer({&source, wpi::NullDeleter<DigitalSource>()}) {}
Tachometer::Tachometer(std::shared_ptr<DigitalSource> source) {
  if (source == nullptr) {
    throw FRC_MakeError(err::NullParameter, "source");
  }

  m_source = source;

  int32_t status = 0;
  HAL_SetCounterUpSource(m_handle, m_source->GetPortHandleForRouting(),
                         static_cast<HAL_AnalogTriggerType>(
                             m_source->GetAnalogTriggerTypeForRouting()),
                         &status);
  FRC_CheckErrorStatus(status, "{}", m_index);
  HAL_SetCounterUpSourceEdge(m_handle, true, false, &status);
  FRC_CheckErrorStatus(status, "{}", m_index);

  HAL_Report(HALUsageReporting::kResourceType_Counter, m_index + 1);
  wpi::SendableRegistry::AddLW(this, "Tachometer", m_index);
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
  FRC_CheckErrorStatus(status, "Channel {}", m_source->GetChannel());
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
  FRC_CheckErrorStatus(status, "Channel {}", m_source->GetChannel());
  return stopped;
}

int Tachometer::GetSamplesToAverage() const {
  int32_t status = 0;
  int32_t samplesToAverage = HAL_GetCounterSamplesToAverage(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_source->GetChannel());
  return samplesToAverage;
}

void Tachometer::SetSamplesToAverage(int samples) {
  int32_t status = 0;
  HAL_SetCounterSamplesToAverage(m_handle, samples, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_source->GetChannel());
}

void Tachometer::SetMaxPeriod(units::second_t maxPeriod) {
  int32_t status = 0;
  HAL_SetCounterMaxPeriod(m_handle, maxPeriod.value(), &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_source->GetChannel());
}

void Tachometer::SetUpdateWhenEmpty(bool updateWhenEmpty) {
  int32_t status = 0;
  HAL_SetCounterUpdateWhenEmpty(m_handle, updateWhenEmpty, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_source->GetChannel());
}

void Tachometer::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Tachometer");
  builder.AddDoubleProperty(
      "RPS", [&] { return GetRevolutionsPerSecond().value(); }, nullptr);
  builder.AddDoubleProperty(
      "RPM", [&] { return GetRevolutionsPerMinute().value(); }, nullptr);
}

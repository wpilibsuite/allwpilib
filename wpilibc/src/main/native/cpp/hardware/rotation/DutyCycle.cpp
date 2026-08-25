// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/rotation/DutyCycle.hpp"

#include <string>

#include "wpi/hal/DutyCycle.h"
#include "wpi/system/Errors.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpi/util/SensorUtil.hpp"
#include "wpi/util/StackTrace.hpp"
#include "wpi/util/UsageReporting.hpp"

using namespace wpi;

DutyCycle::DutyCycle(int channel) : m_channel{channel} {
  InitDutyCycle();
}

void DutyCycle::InitDutyCycle() {
  int32_t status = 0;
  std::string stackTrace = wpi::util::GetStackTrace(1);
  m_handle = HAL_InitializeDutyCycle(m_channel, stackTrace.c_str(), &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  wpi::util::ReportUsage("IO", m_channel, "DutyCycle");
}

wpi::units::hertz<> DutyCycle::GetFrequency() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleFrequency(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return wpi::units::hertz<>{retVal};
}

double DutyCycle::GetOutput() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleOutput(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return retVal;
}

wpi::units::seconds<> DutyCycle::GetHighTime() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleHighTime(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return wpi::units::nanoseconds<>{static_cast<double>(retVal)};
}

int DutyCycle::GetSourceChannel() const {
  return m_channel;
}

void DutyCycle::LogTo(wpi::telemetry::TelemetryTable& table) const {
  table.Log("Frequency", GetFrequency());
  table.Log("Output", GetOutput());
}

std::string_view DutyCycle::GetTelemetryType() const {
  return "Duty Cycle";
}

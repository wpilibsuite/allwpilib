// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/rotation/DutyCycle.hpp"

#include <string>

#include "wpi/hal/DutyCycle.h"
#include "wpi/hal/UsageReporting.h"
#include "wpi/system/Errors.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpi/util/SensorUtil.hpp"
#include "wpi/util/StackTrace.hpp"

using namespace wpi;

DutyCycle::DutyCycle(int channel) : m_channel{channel} {
  if (!SensorUtil::CheckDigitalChannel(channel)) {
    throw WPILIB_MakeError(err::ChannelIndexOutOfRange, "Channel {}", channel);
  }
  InitDutyCycle();
}

void DutyCycle::InitDutyCycle() {
  int32_t status = 0;
  std::string stackTrace = wpi::util::GetStackTrace(1);
  m_handle = HAL_InitializeDutyCycle(m_channel, stackTrace.c_str(), &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  HAL_ReportUsage("IO", m_channel, "DutyCycle");
}

wpi::units::hertz_t DutyCycle::GetFrequency() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleFrequency(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return wpi::units::hertz_t{retVal};
}

double DutyCycle::GetOutput() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleOutput(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return retVal;
}

wpi::units::second_t DutyCycle::GetHighTime() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleHighTime(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return wpi::units::nanosecond_t{static_cast<double>(retVal)};
}

int DutyCycle::GetSourceChannel() const {
  return m_channel;
}

void DutyCycle::LogTo(wpi::TelemetryTable& table) const {
  table.Log("Frequency", GetFrequency());
  table.Log("Output", GetOutput());
}

std::string_view DutyCycle::GetTelemetryType() const {
  return "Duty Cycle";
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/discrete/DigitalOutput.hpp"

#include <string>

#include "wpi/hal/DIO.h"
#include "wpi/system/Errors.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpi/util/SensorUtil.hpp"
#include "wpi/util/StackTrace.hpp"
#include "wpi/util/UsageReporting.hpp"

using namespace wpi;

DigitalOutput::DigitalOutput(int channel) {
  m_channel = channel;

  int32_t status = 0;
  std::string stackTrace = wpi::util::GetStackTrace(1);
  m_handle = HAL_InitializeDIOPort(channel, false, stackTrace.c_str(), &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", channel);

  wpi::util::ReportUsage("IO", channel, "DigitalOutput");
}

DigitalOutput::~DigitalOutput() {}

void DigitalOutput::Set(bool value) {
  int32_t status = 0;
  HAL_SetDIO(m_handle, value, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);
}

bool DigitalOutput::Get() const {
  int32_t status = 0;
  bool val = HAL_GetDIO(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);
  return val;
}

int DigitalOutput::GetChannel() const {
  return m_channel;
}

void DigitalOutput::SetSimDevice(HAL_SimDeviceHandle device) {
  HAL_SetDIOSimDevice(m_handle, device);
}

void DigitalOutput::LogTo(wpi::telemetry::TelemetryTable& table) const {
  table.Log("Value", Get());
}

std::string_view DigitalOutput::GetTelemetryType() const {
  return "Digital Output";
}

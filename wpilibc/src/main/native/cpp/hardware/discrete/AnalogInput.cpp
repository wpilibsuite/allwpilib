// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/discrete/AnalogInput.hpp"

#include <string>

#include "wpi/hal/AnalogInput.h"
#include "wpi/hal/UsageReporting.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/util/SensorUtil.hpp"
#include "wpi/util/StackTrace.hpp"
#include "wpi/util/sendable/SendableBuilder.hpp"
#include "wpi/util/sendable/SendableRegistry.hpp"

using namespace wpi;

AnalogInput::AnalogInput(int channel) {
  m_channel = channel;
  int32_t status = 0;
  std::string stackTrace = wpi::util::GetStackTrace(1);
  m_port = HAL_InitializeAnalogInputPort(channel, stackTrace.c_str(), &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", channel);

  HAL_ReportUsage("IO", channel, "AnalogInput");

  wpi::util::SendableRegistry::Add(this, "AnalogInput", channel);
}

int AnalogInput::GetValue() const {
  int32_t status = 0;
  int value = HAL_GetAnalogValue(m_port, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);
  return value;
}

double AnalogInput::GetVoltage() const {
  int32_t status = 0;
  double voltage = HAL_GetAnalogVoltage(m_port, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);
  return voltage;
}

int AnalogInput::GetChannel() const {
  return m_channel;
}

void AnalogInput::SetSimDevice(HAL_SimDeviceHandle device) {
  HAL_SetAnalogInputSimDevice(m_port, device);
}

void AnalogInput::InitSendable(wpi::util::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Analog Input");
  builder.AddDoubleProperty(
      "Value", [=, this] { return GetVoltage(); }, nullptr);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/AnalogInput.h"

#include <string>

#include <hal/AnalogInput.h>
#include <hal/HALBase.h>
#include <hal/Ports.h>
#include <hal/UsageReporting.h>
#include <wpi/StackTrace.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"
#include "frc/SensorUtil.h"
#include "frc/Timer.h"

using namespace frc;

AnalogInput::AnalogInput(int channel) {
  m_channel = channel;
  int32_t status = 0;
  std::string stackTrace = wpi::GetStackTrace(1);
  m_port = HAL_InitializeAnalogInputPort(channel, stackTrace.c_str(), &status);
  FRC_CheckErrorStatus(status, "Channel {}", channel);

  HAL_ReportUsage("IO", channel, "AnalogInput");

  wpi::SendableRegistry::Add(this, "AnalogInput", channel);
}

int AnalogInput::GetValue() const {
  int32_t status = 0;
  int value = HAL_GetAnalogValue(m_port, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return value;
}

double AnalogInput::GetVoltage() const {
  int32_t status = 0;
  double voltage = HAL_GetAnalogVoltage(m_port, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return voltage;
}

int AnalogInput::GetChannel() const {
  return m_channel;
}

void AnalogInput::SetSimDevice(HAL_SimDeviceHandle device) {
  HAL_SetAnalogInputSimDevice(m_port, device);
}

void AnalogInput::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Analog Input");
  builder.AddDoubleProperty(
      "Value", [=, this] { return GetVoltage(); }, nullptr);
}

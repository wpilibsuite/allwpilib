// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DigitalInput.h"

#include <string>

#include <hal/DIO.h>
#include <hal/HALBase.h>
#include <hal/Ports.h>
#include <hal/UsageReporting.h>
#include <wpi/StackTrace.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"
#include "frc/SensorUtil.h"

using namespace frc;

DigitalInput::DigitalInput(int channel) {
  if (!SensorUtil::CheckDigitalChannel(channel)) {
    throw FRC_MakeError(err::ChannelIndexOutOfRange, "Channel {}", channel);
  }
  m_channel = channel;

  int32_t status = 0;
  std::string stackTrace = wpi::GetStackTrace(1);
  m_handle = HAL_InitializeDIOPort(channel, true, stackTrace.c_str(), &status);
  FRC_CheckErrorStatus(status, "Channel {}", channel);

  HAL_ReportUsage("DigitalInput", channel + 1, "");
  wpi::SendableRegistry::Add(this, "DigitalInput", channel);
}

bool DigitalInput::Get() const {
  int32_t status = 0;
  bool value = HAL_GetDIO(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return value;
}

void DigitalInput::SetSimDevice(HAL_SimDeviceHandle device) {
  HAL_SetDIOSimDevice(m_handle, device);
}

int DigitalInput::GetChannel() const {
  return m_channel;
}

void DigitalInput::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Digital Input");
  builder.AddBooleanProperty("Value", [=, this] { return Get(); }, nullptr);
}

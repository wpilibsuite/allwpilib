// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/discrete/DigitalInput.hpp"

#include <string>

#include <wpi/hal/DIO.h>
#include <wpi/hal/HALBase.h>
#include <wpi/hal/Ports.h>
#include <wpi/hal/UsageReporting.h>
#include <wpi/util/SensorUtil.hpp>
#include <wpi/util/StackTrace.hpp>
#include <wpi/util/sendable/SendableBuilder.hpp>
#include <wpi/util/sendable/SendableRegistry.hpp>

#include "wpi/system/Errors.hpp"

using namespace wpi;

DigitalInput::DigitalInput(int channel) {
  if (!SensorUtil::CheckDigitalChannel(channel)) {
    throw WPILIB_MakeError(err::ChannelIndexOutOfRange, "Channel {}", channel);
  }
  m_channel = channel;

  int32_t status = 0;
  std::string stackTrace = wpi::util::GetStackTrace(1);
  m_handle = HAL_InitializeDIOPort(channel, true, stackTrace.c_str(), &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", channel);

  HAL_ReportUsage("IO", channel, "DigitalInput");
  wpi::util::SendableRegistry::Add(this, "DigitalInput", channel);
}

bool DigitalInput::Get() const {
  int32_t status = 0;
  bool value = HAL_GetDIO(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);
  return value;
}

void DigitalInput::SetSimDevice(HAL_SimDeviceHandle device) {
  HAL_SetDIOSimDevice(m_handle, device);
}

int DigitalInput::GetChannel() const {
  return m_channel;
}

void DigitalInput::InitSendable(wpi::util::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Digital Input");
  builder.AddBooleanProperty("Value", [=, this] { return Get(); }, nullptr);
}

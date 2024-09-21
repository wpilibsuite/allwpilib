// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/AnalogOutput.h"

#include <string>

#include <hal/AnalogOutput.h>
#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <hal/Ports.h>
#include <wpi/StackTrace.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"
#include "frc/SensorUtil.h"

using namespace frc;

AnalogOutput::AnalogOutput(int channel) {
  if (!SensorUtil::CheckAnalogOutputChannel(channel)) {
    throw FRC_MakeError(err::ChannelIndexOutOfRange, "Channel {}", channel);
  }

  m_channel = channel;

  HAL_PortHandle port = HAL_GetPort(m_channel);
  int32_t status = 0;
  std::string stackTrace = wpi::GetStackTrace(1);
  m_port = HAL_InitializeAnalogOutputPort(port, stackTrace.c_str(), &status);
  FRC_CheckErrorStatus(status, "Channel {}", channel);

  HAL_Report(HALUsageReporting::kResourceType_AnalogOutput, m_channel + 1);
  wpi::SendableRegistry::AddLW(this, "AnalogOutput", m_channel);
}

void AnalogOutput::SetVoltage(double voltage) {
  int32_t status = 0;
  HAL_SetAnalogOutput(m_port, voltage, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

double AnalogOutput::GetVoltage() const {
  int32_t status = 0;
  double voltage = HAL_GetAnalogOutput(m_port, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return voltage;
}

int AnalogOutput::GetChannel() const {
  return m_channel;
}

void AnalogOutput::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Analog Output");
  builder.AddDoubleProperty(
      "Value", [=, this] { return GetVoltage(); },
      [=, this](double value) { SetVoltage(value); });
}

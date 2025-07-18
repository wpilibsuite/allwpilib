// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DutyCycle.h"

#include <string>
#include <utility>

#include <hal/DutyCycle.h>
#include <hal/HALBase.h>
#include <hal/UsageReporting.h>
#include <wpi/NullDeleter.h>
#include <wpi/StackTrace.h>
#include <wpi/sendable/SendableBuilder.h>

#include "frc/Errors.h"
#include "frc/SensorUtil.h"

using namespace frc;

DutyCycle::DutyCycle(int channel) : m_channel{channel} {
  if (!SensorUtil::CheckDigitalChannel(channel)) {
    throw FRC_MakeError(err::ChannelIndexOutOfRange, "Channel {}", channel);
  }
  InitDutyCycle();
}

void DutyCycle::InitDutyCycle() {
  int32_t status = 0;
  std::string stackTrace = wpi::GetStackTrace(1);
  m_handle = HAL_InitializeDutyCycle(m_channel, stackTrace.c_str(), &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  HAL_ReportUsage("IO", m_channel, "DutyCycle");
  wpi::SendableRegistry::Add(this, "Duty Cycle", m_channel);
}

units::hertz_t DutyCycle::GetFrequency() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleFrequency(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return units::hertz_t{retVal};
}

double DutyCycle::GetOutput() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleOutput(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return retVal;
}

units::second_t DutyCycle::GetHighTime() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleHighTime(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return units::nanosecond_t{static_cast<double>(retVal)};
}

int DutyCycle::GetSourceChannel() const {
  return m_channel;
}

void DutyCycle::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Duty Cycle");
  builder.AddDoubleProperty(
      "Frequency", [this] { return this->GetFrequency().value(); }, nullptr);
  builder.AddDoubleProperty(
      "Output", [this] { return this->GetOutput(); }, nullptr);
}

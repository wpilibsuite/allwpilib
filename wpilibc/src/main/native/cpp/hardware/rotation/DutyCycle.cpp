// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/rotation/DutyCycleInput.hpp"

#include <string>
#include <utility>

#include "wpi/hal/DutyCycleInput.h"
#include "wpi/hal/HALBase.h"
#include "wpi/hal/UsageReporting.h"
#include "wpi/system/Errors.hpp"
#include "wpi/util/NullDeleter.hpp"
#include "wpi/util/SensorUtil.hpp"
#include "wpi/util/StackTrace.hpp"
#include "wpi/util/sendable/SendableBuilder.hpp"

using namespace wpi;

DutyCycleInput::DutyCycleInput(int channel) : m_channel{channel} {
  if (!SensorUtil::CheckDigitalChannel(channel)) {
    throw WPILIB_MakeError(err::ChannelIndexOutOfRange, "Channel {}", channel);
  }
  InitDutyCycle();
}

void DutyCycleInput::InitDutyCycle() {
  int32_t status = 0;
  std::string stackTrace = wpi::util::GetStackTrace(1);
  m_handle = HAL_InitializeDutyCycle(m_channel, stackTrace.c_str(), &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  HAL_ReportUsage("IO", m_channel, "DutyCycleInput");
  wpi::util::SendableRegistry::Add(this, "Duty Cycle", m_channel);
}

wpi::units::hertz_t DutyCycleInput::GetFrequency() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleFrequency(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return wpi::units::hertz_t{retVal};
}

double DutyCycleInput::GetOutput() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleOutput(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return retVal;
}

wpi::units::second_t DutyCycleInput::GetHighTime() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleHighTime(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return wpi::units::nanosecond_t{static_cast<double>(retVal)};
}

int DutyCycleInput::GetSourceChannel() const {
  return m_channel;
}

void DutyCycleInput::InitSendable(wpi::util::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Duty Cycle");
  builder.AddDoubleProperty(
      "Frequency", [this] { return this->GetFrequency().value(); }, nullptr);
  builder.AddDoubleProperty(
      "Output", [this] { return this->GetOutput(); }, nullptr);
}

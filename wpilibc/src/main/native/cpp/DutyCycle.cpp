// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DutyCycle.h"

#include <string>
#include <utility>

#include <hal/DutyCycle.h>
#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <wpi/NullDeleter.h>
#include <wpi/StackTrace.h>
#include <wpi/sendable/SendableBuilder.h>

#include "frc/DigitalSource.h"
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
  m_handle = HAL_InitializeDutyCycle(HAL_GetPort(m_channel), stackTrace.c_str(),
                                     &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  HAL_Report(HALUsageReporting::kResourceType_DutyCycle, m_channel + 1);
  wpi::SendableRegistry::Add(this, "Duty Cycle", m_channel);
}

int DutyCycle::GetFPGAIndex() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleFPGAIndex(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return retVal;
}

int DutyCycle::GetFrequency() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleFrequency(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return retVal;
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

unsigned int DutyCycle::GetOutputScaleFactor() const {
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleOutputScaleFactor(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", GetSourceChannel());
  return retVal;
}

int DutyCycle::GetSourceChannel() const {
  return m_channel;
}

void DutyCycle::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Duty Cycle");
  builder.AddDoubleProperty(
      "Frequency", [this] { return this->GetFrequency(); }, nullptr);
  builder.AddDoubleProperty(
      "Output", [this] { return this->GetOutput(); }, nullptr);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/AnalogInput.h"

#include <string>

#include <hal/AnalogAccumulator.h>
#include <hal/AnalogInput.h>
#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <hal/Ports.h>
#include <wpi/StackTrace.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"
#include "frc/SensorUtil.h"
#include "frc/Timer.h"

using namespace frc;

AnalogInput::AnalogInput(int channel) {
  if (!SensorUtil::CheckAnalogInputChannel(channel)) {
    throw FRC_MakeError(err::ChannelIndexOutOfRange, "Channel {}", channel);
  }

  m_channel = channel;

  HAL_PortHandle port = HAL_GetPort(channel);
  int32_t status = 0;
  std::string stackTrace = wpi::GetStackTrace(1);
  m_port = HAL_InitializeAnalogInputPort(port, stackTrace.c_str(), &status);
  FRC_CheckErrorStatus(status, "Channel {}", channel);

  HAL_Report(HALUsageReporting::kResourceType_AnalogChannel, channel + 1);

  wpi::SendableRegistry::AddLW(this, "AnalogInput", channel);
}

int AnalogInput::GetValue() const {
  int32_t status = 0;
  int value = HAL_GetAnalogValue(m_port, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return value;
}

int AnalogInput::GetAverageValue() const {
  int32_t status = 0;
  int value = HAL_GetAnalogAverageValue(m_port, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return value;
}

double AnalogInput::GetVoltage() const {
  int32_t status = 0;
  double voltage = HAL_GetAnalogVoltage(m_port, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return voltage;
}

double AnalogInput::GetAverageVoltage() const {
  int32_t status = 0;
  double voltage = HAL_GetAnalogAverageVoltage(m_port, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return voltage;
}

int AnalogInput::GetChannel() const {
  return m_channel;
}

void AnalogInput::SetAverageBits(int bits) {
  int32_t status = 0;
  HAL_SetAnalogAverageBits(m_port, bits, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

int AnalogInput::GetAverageBits() const {
  int32_t status = 0;
  int averageBits = HAL_GetAnalogAverageBits(m_port, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return averageBits;
}

void AnalogInput::SetOversampleBits(int bits) {
  int32_t status = 0;
  HAL_SetAnalogOversampleBits(m_port, bits, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

int AnalogInput::GetOversampleBits() const {
  int32_t status = 0;
  int oversampleBits = HAL_GetAnalogOversampleBits(m_port, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return oversampleBits;
}

int AnalogInput::GetLSBWeight() const {
  int32_t status = 0;
  int lsbWeight = HAL_GetAnalogLSBWeight(m_port, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return lsbWeight;
}

int AnalogInput::GetOffset() const {
  int32_t status = 0;
  int offset = HAL_GetAnalogOffset(m_port, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return offset;
}

bool AnalogInput::IsAccumulatorChannel() const {
  int32_t status = 0;
  bool isAccum = HAL_IsAccumulatorChannel(m_port, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return isAccum;
}

void AnalogInput::InitAccumulator() {
  m_accumulatorOffset = 0;
  int32_t status = 0;
  HAL_InitAccumulator(m_port, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

void AnalogInput::SetAccumulatorInitialValue(int64_t initialValue) {
  m_accumulatorOffset = initialValue;
}

void AnalogInput::ResetAccumulator() {
  int32_t status = 0;
  HAL_ResetAccumulator(m_port, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);

  // Wait until the next sample, so the next call to GetAccumulator*()
  // won't have old values.
  const double sampleTime = 1.0 / GetSampleRate();
  const double overSamples = 1 << GetOversampleBits();
  const double averageSamples = 1 << GetAverageBits();
  Wait(units::second_t{sampleTime * overSamples * averageSamples});
}

void AnalogInput::SetAccumulatorCenter(int center) {
  int32_t status = 0;
  HAL_SetAccumulatorCenter(m_port, center, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

void AnalogInput::SetAccumulatorDeadband(int deadband) {
  int32_t status = 0;
  HAL_SetAccumulatorDeadband(m_port, deadband, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

int64_t AnalogInput::GetAccumulatorValue() const {
  int32_t status = 0;
  int64_t value = HAL_GetAccumulatorValue(m_port, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return value + m_accumulatorOffset;
}

int64_t AnalogInput::GetAccumulatorCount() const {
  int32_t status = 0;
  int64_t count = HAL_GetAccumulatorCount(m_port, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return count;
}

void AnalogInput::GetAccumulatorOutput(int64_t& value, int64_t& count) const {
  int32_t status = 0;
  HAL_GetAccumulatorOutput(m_port, &value, &count, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  value += m_accumulatorOffset;
}

void AnalogInput::SetSampleRate(double samplesPerSecond) {
  int32_t status = 0;
  HAL_SetAnalogSampleRate(samplesPerSecond, &status);
  FRC_CheckErrorStatus(status, "SetSampleRate");
}

double AnalogInput::GetSampleRate() {
  int32_t status = 0;
  double sampleRate = HAL_GetAnalogSampleRate(&status);
  FRC_CheckErrorStatus(status, "GetSampleRate");
  return sampleRate;
}

void AnalogInput::SetSimDevice(HAL_SimDeviceHandle device) {
  HAL_SetAnalogInputSimDevice(m_port, device);
}

void AnalogInput::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Analog Input");
  builder.AddDoubleProperty(
      "Value", [=, this] { return GetAverageVoltage(); }, nullptr);
}

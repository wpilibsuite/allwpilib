// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/discrete/DigitalOutput.hpp"

#include <string>

#include "wpi/hal/DIO.h"
#include "wpi/hal/UsageReporting.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/util/SensorUtil.hpp"
#include "wpi/util/StackTrace.hpp"
#include "wpi/util/sendable/SendableBuilder.hpp"
#include "wpi/util/sendable/SendableRegistry.hpp"

using namespace wpi;

DigitalOutput::DigitalOutput(int channel) {
  m_pwmGenerator = HAL_kInvalidHandle;
  if (!SensorUtil::CheckDigitalChannel(channel)) {
    throw WPILIB_MakeError(err::ChannelIndexOutOfRange, "Channel {}", channel);
  }
  m_channel = channel;

  int32_t status = 0;
  std::string stackTrace = wpi::util::GetStackTrace(1);
  m_handle = HAL_InitializeDIOPort(channel, false, stackTrace.c_str(), &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", channel);

  HAL_ReportUsage("IO", channel, "DigitalOutput");
  wpi::util::SendableRegistry::Add(this, "DigitalOutput", channel);
}

DigitalOutput::~DigitalOutput() {
  if (m_handle != HAL_kInvalidHandle) {
    // Disable the PWM in case it was running.
    try {
      DisablePWM();
    } catch (const RuntimeError& e) {
      e.Report();
    }
  }
}

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

void DigitalOutput::Pulse(wpi::units::second_t pulseLength) {
  int32_t status = 0;
  HAL_Pulse(m_handle, pulseLength.value(), &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);
}

bool DigitalOutput::IsPulsing() const {
  int32_t status = 0;
  bool value = HAL_IsPulsing(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);
  return value;
}

void DigitalOutput::SetPWMRate(double rate) {
  int32_t status = 0;
  HAL_SetDigitalPWMRate(rate, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);
}

void DigitalOutput::EnablePPS(double dutyCycle) {
  if (m_pwmGenerator != HAL_kInvalidHandle) {
    return;
  }

  int32_t status = 0;

  m_pwmGenerator = HAL_AllocateDigitalPWM(&status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);

  HAL_SetDigitalPWMPPS(m_pwmGenerator, dutyCycle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);

  HAL_SetDigitalPWMOutputChannel(m_pwmGenerator, m_channel, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);
}

void DigitalOutput::EnablePWM(double initialDutyCycle) {
  if (m_pwmGenerator != HAL_kInvalidHandle) {
    return;
  }

  int32_t status = 0;

  m_pwmGenerator = HAL_AllocateDigitalPWM(&status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);

  HAL_SetDigitalPWMDutyCycle(m_pwmGenerator, initialDutyCycle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);

  HAL_SetDigitalPWMOutputChannel(m_pwmGenerator, m_channel, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);
}

void DigitalOutput::DisablePWM() {
  if (m_pwmGenerator == HAL_kInvalidHandle) {
    return;
  }

  int32_t status = 0;

  // Disable the output by routing to a dead bit.
  HAL_SetDigitalPWMOutputChannel(m_pwmGenerator,
                                 SensorUtil::GetNumDigitalChannels(), &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);

  HAL_FreeDigitalPWM(m_pwmGenerator);

  m_pwmGenerator = HAL_kInvalidHandle;
}

void DigitalOutput::UpdateDutyCycle(double dutyCycle) {
  int32_t status = 0;
  HAL_SetDigitalPWMDutyCycle(m_pwmGenerator, dutyCycle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);
}

void DigitalOutput::SetSimDevice(HAL_SimDeviceHandle device) {
  HAL_SetDIOSimDevice(m_handle, device);
}

void DigitalOutput::InitSendable(wpi::util::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Digital Output");
  builder.AddBooleanProperty(
      "Value", [=, this] { return Get(); },
      [=, this](bool value) { Set(value); });
}

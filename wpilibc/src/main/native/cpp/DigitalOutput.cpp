/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/DigitalOutput.h"

#include <limits>

#include <hal/DIO.h>
#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <hal/Ports.h>

#include "frc/SensorUtil.h"
#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

DigitalOutput::DigitalOutput(int channel) {
  m_pwmGenerator = HAL_kInvalidHandle;
  if (!SensorUtil::CheckDigitalChannel(channel)) {
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange,
                               "Digital Channel " + wpi::Twine(channel));
    m_channel = std::numeric_limits<int>::max();
    return;
  }
  m_channel = channel;

  int32_t status = 0;
  m_handle = HAL_InitializeDIOPort(HAL_GetPort(channel), false, &status);
  if (status != 0) {
    wpi_setHALErrorWithRange(status, 0, HAL_GetNumDigitalChannels(), channel);
    m_channel = std::numeric_limits<int>::max();
    m_handle = HAL_kInvalidHandle;
    return;
  }

  HAL_Report(HALUsageReporting::kResourceType_DigitalOutput, channel + 1);
  SendableRegistry::GetInstance().AddLW(this, "DigitalOutput", channel);
}

DigitalOutput::~DigitalOutput() {
  if (StatusIsFatal()) return;
  // Disable the PWM in case it was running.
  DisablePWM();

  HAL_FreeDIOPort(m_handle);
}

void DigitalOutput::Set(bool value) {
  if (StatusIsFatal()) return;

  int32_t status = 0;
  HAL_SetDIO(m_handle, value, &status);
  wpi_setHALError(status);
}

bool DigitalOutput::Get() const {
  if (StatusIsFatal()) return false;

  int32_t status = 0;
  bool val = HAL_GetDIO(m_handle, &status);
  wpi_setHALError(status);
  return val;
}

HAL_Handle DigitalOutput::GetPortHandleForRouting() const { return m_handle; }

AnalogTriggerType DigitalOutput::GetAnalogTriggerTypeForRouting() const {
  return (AnalogTriggerType)0;
}

bool DigitalOutput::IsAnalogTrigger() const { return false; }

int DigitalOutput::GetChannel() const { return m_channel; }

void DigitalOutput::Pulse(double length) {
  if (StatusIsFatal()) return;

  int32_t status = 0;
  HAL_Pulse(m_handle, length, &status);
  wpi_setHALError(status);
}

bool DigitalOutput::IsPulsing() const {
  if (StatusIsFatal()) return false;

  int32_t status = 0;
  bool value = HAL_IsPulsing(m_handle, &status);
  wpi_setHALError(status);
  return value;
}

void DigitalOutput::SetPWMRate(double rate) {
  if (StatusIsFatal()) return;

  int32_t status = 0;
  HAL_SetDigitalPWMRate(rate, &status);
  wpi_setHALError(status);
}

void DigitalOutput::EnablePWM(double initialDutyCycle) {
  if (m_pwmGenerator != HAL_kInvalidHandle) return;

  int32_t status = 0;

  if (StatusIsFatal()) return;
  m_pwmGenerator = HAL_AllocateDigitalPWM(&status);
  wpi_setHALError(status);

  if (StatusIsFatal()) return;
  HAL_SetDigitalPWMDutyCycle(m_pwmGenerator, initialDutyCycle, &status);
  wpi_setHALError(status);

  if (StatusIsFatal()) return;
  HAL_SetDigitalPWMOutputChannel(m_pwmGenerator, m_channel, &status);
  wpi_setHALError(status);
}

void DigitalOutput::DisablePWM() {
  if (StatusIsFatal()) return;
  if (m_pwmGenerator == HAL_kInvalidHandle) return;

  int32_t status = 0;

  // Disable the output by routing to a dead bit.
  HAL_SetDigitalPWMOutputChannel(m_pwmGenerator, SensorUtil::kDigitalChannels,
                                 &status);
  wpi_setHALError(status);

  HAL_FreeDigitalPWM(m_pwmGenerator, &status);
  wpi_setHALError(status);

  m_pwmGenerator = HAL_kInvalidHandle;
}

void DigitalOutput::UpdateDutyCycle(double dutyCycle) {
  if (StatusIsFatal()) return;

  int32_t status = 0;
  HAL_SetDigitalPWMDutyCycle(m_pwmGenerator, dutyCycle, &status);
  wpi_setHALError(status);
}

void DigitalOutput::SetSimDevice(HAL_SimDeviceHandle device) {
  HAL_SetDIOSimDevice(m_handle, device);
}

void DigitalOutput::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Digital Output");
  builder.AddBooleanProperty(
      "Value", [=]() { return Get(); }, [=](bool value) { Set(value); });
}

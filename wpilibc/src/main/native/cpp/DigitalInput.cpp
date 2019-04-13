/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/DigitalInput.h"

#include <limits>
#include <utility>

#include <hal/DIO.h>
#include <hal/HAL.h>
#include <hal/Ports.h>

#include "frc/SensorUtil.h"
#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

DigitalInput::DigitalInput(int channel) {
  if (!SensorUtil::CheckDigitalChannel(channel)) {
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange,
                               "Digital Channel " + wpi::Twine(channel));
    m_channel = std::numeric_limits<int>::max();
    return;
  }
  m_channel = channel;

  int32_t status = 0;
  m_handle = HAL_InitializeDIOPort(HAL_GetPort(channel), true, &status);
  if (status != 0) {
    wpi_setErrorWithContextRange(status, 0, HAL_GetNumDigitalChannels(),
                                 channel, HAL_GetErrorMessage(status));
    m_handle = HAL_kInvalidHandle;
    m_channel = std::numeric_limits<int>::max();
    return;
  }

  HAL_Report(HALUsageReporting::kResourceType_DigitalInput, channel);
  SetName("DigitalInput", channel);
}

DigitalInput::~DigitalInput() {
  if (StatusIsFatal()) return;
  if (m_interrupt != HAL_kInvalidHandle) {
    int32_t status = 0;
    HAL_CleanInterrupts(m_interrupt, &status);
    // Ignore status, as an invalid handle just needs to be ignored.
    m_interrupt = HAL_kInvalidHandle;
  }

  HAL_FreeDIOPort(m_handle);
}

DigitalInput::DigitalInput(DigitalInput&& rhs)
    : DigitalSource(std::move(rhs)), m_channel(std::move(rhs.m_channel)) {
  std::swap(m_handle, rhs.m_handle);
}

DigitalInput& DigitalInput::operator=(DigitalInput&& rhs) {
  DigitalSource::operator=(std::move(rhs));

  m_channel = std::move(rhs.m_channel);
  std::swap(m_handle, rhs.m_handle);

  return *this;
}

bool DigitalInput::Get() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value = HAL_GetDIO(m_handle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

HAL_Handle DigitalInput::GetPortHandleForRouting() const { return m_handle; }

AnalogTriggerType DigitalInput::GetAnalogTriggerTypeForRouting() const {
  return (AnalogTriggerType)0;
}

bool DigitalInput::IsAnalogTrigger() const { return false; }

int DigitalInput::GetChannel() const { return m_channel; }

void DigitalInput::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Digital Input");
  builder.AddBooleanProperty("Value", [=]() { return Get(); }, nullptr);
}

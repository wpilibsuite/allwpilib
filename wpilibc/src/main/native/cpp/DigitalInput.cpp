/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/DigitalInput.h"

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
    wpi_setHALErrorWithRange(status, 0, HAL_GetNumDigitalChannels(), channel);
    m_handle = HAL_kInvalidHandle;
    m_channel = std::numeric_limits<int>::max();
    return;
  }

  HAL_Report(HALUsageReporting::kResourceType_DigitalInput, channel + 1);
  SendableRegistry::GetInstance().AddLW(this, "DigitalInput", channel);
}

DigitalInput::~DigitalInput() {
  if (StatusIsFatal()) return;
  HAL_FreeDIOPort(m_handle);
}

bool DigitalInput::Get() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value = HAL_GetDIO(m_handle, &status);
  wpi_setHALError(status);
  return value;
}

HAL_Handle DigitalInput::GetPortHandleForRouting() const { return m_handle; }

AnalogTriggerType DigitalInput::GetAnalogTriggerTypeForRouting() const {
  return (AnalogTriggerType)0;
}

bool DigitalInput::IsAnalogTrigger() const { return false; }

void DigitalInput::SetSimDevice(HAL_SimDeviceHandle device) {
  HAL_SetDIOSimDevice(m_handle, device);
}

int DigitalInput::GetChannel() const { return m_channel; }

void DigitalInput::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Digital Input");
  builder.AddBooleanProperty(
      "Value", [=]() { return Get(); }, nullptr);
}

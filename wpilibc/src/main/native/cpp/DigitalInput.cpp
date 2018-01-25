/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DigitalInput.h"

#include <limits>

#include <HAL/DIO.h>
#include <HAL/HAL.h>
#include <HAL/Ports.h>

#include "SmartDashboard/SendableBuilder.h"
#include "WPIErrors.h"

using namespace frc;

/**
 * Create an instance of a Digital Input class.
 *
 * Creates a digital input given a channel.
 *
 * @param channel The DIO channel 0-9 are on-board, 10-25 are on the MXP port
 */
DigitalInput::DigitalInput(int channel) {
  if (!CheckDigitalChannel(channel)) {
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange,
                               "Digital Channel " + llvm::Twine(channel));
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

/**
 * Free resources associated with the Digital Input class.
 */
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

/**
 * Get the value from a digital input channel.
 *
 * Retrieve the value of a single digital input channel from the FPGA.
 */
bool DigitalInput::Get() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value = HAL_GetDIO(m_handle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

/**
 * @return The GPIO channel number that this object represents.
 */
int DigitalInput::GetChannel() const { return m_channel; }

/**
 * @return The HAL Handle to the specified source.
 */
HAL_Handle DigitalInput::GetPortHandleForRouting() const { return m_handle; }

/**
 * Is source an AnalogTrigger
 */
bool DigitalInput::IsAnalogTrigger() const { return false; }

/**
 * @return The type of analog trigger output to be used. 0 for Digitals
 */
AnalogTriggerType DigitalInput::GetAnalogTriggerTypeForRouting() const {
  return (AnalogTriggerType)0;
}

void DigitalInput::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Digital Input");
  builder.AddBooleanProperty("Value", [=]() { return Get(); }, nullptr);
}

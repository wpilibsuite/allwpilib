/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "DigitalInput.h"
#include "Resource.h"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"

#include <limits>
#include <sstream>

/**
 * Create an instance of a Digital Input class.
 * Creates a digital input given a channel.
 *
 * @param channel The DIO channel 0-9 are on-board, 10-25 are on the MXP port
 */
DigitalInput::DigitalInput(uint32_t channel) {
  std::stringstream buf;

  if (!CheckDigitalChannel(channel)) {
    buf << "Digital Channel " << channel;
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf.str());
    m_channel = std::numeric_limits<uint32_t>::max();
    return;
  }
  m_channel = channel;

  int32_t status = 0;
  allocateDIO(m_digital_ports[channel], true, &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));

  LiveWindow::GetInstance()->AddSensor("DigitalInput", channel, this);
  HALReport(HALUsageReporting::kResourceType_DigitalInput, channel);
}

/**
 * Free resources associated with the Digital Input class.
 */
DigitalInput::~DigitalInput() {
  if (StatusIsFatal()) return;
  if (m_interrupt != nullptr) {
    int32_t status = 0;
    cleanInterrupts(m_interrupt, &status);
    wpi_setErrorWithContext(status, getHALErrorMessage(status));
    m_interrupt = nullptr;
    m_interrupts->Free(m_interruptIndex);
  }

  int32_t status = 0;
  freeDIO(m_digital_ports[m_channel], &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

/**
 * Get the value from a digital input channel.
 * Retrieve the value of a single digital input channel from the FPGA.
 */
bool DigitalInput::Get() const {
  if (StatusIsFatal()) return false;
  int32_t status = 0;
  bool value = getDIO(m_digital_ports[m_channel], &status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return value;
}

/**
 * @return The GPIO channel number that this object represents.
 */
uint32_t DigitalInput::GetChannel() const { return m_channel; }

/**
 * @return The value to be written to the channel field of a routing mux.
 */
uint32_t DigitalInput::GetChannelForRouting() const { return GetChannel(); }

/**
 * @return The value to be written to the module field of a routing mux.
 */
uint32_t DigitalInput::GetModuleForRouting() const { return 0; }

/**
 * @return The value to be written to the analog trigger field of a routing mux.
 */
bool DigitalInput::GetAnalogTriggerForRouting() const { return false; }

void DigitalInput::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutBoolean("Value", Get());
  }
}

void DigitalInput::StartLiveWindowMode() {}

void DigitalInput::StopLiveWindowMode() {}

std::string DigitalInput::GetSmartDashboardType() const {
  return "DigitalInput";
}

void DigitalInput::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> DigitalInput::GetTable() const { return m_table; }

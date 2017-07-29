/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DigitalInput.h"

#include <sstream>

#include "WPIErrors.h"

/**
 * Create an instance of a Digital Input class.
 * Creates a digital input given a channel and uses the default module.
 *
 * @param channel The digital channel (1..14).
 */
frc::DigitalInput::DigitalInput(int channel) : m_channel(channel) {
  std::ostringstream oss;
  oss << "dio/" << channel;
  m_impl = new SimDigitalInput(oss.str());
}

/**
 * Get the value from a digital input channel.
 * Retrieve the value of a single digital input channel from the FPGA.
 */
int frc::DigitalInput::Get() const { return m_impl->Get(); }

/**
 * @return The GPIO channel number that this object represents.
 */
int frc::DigitalInput::GetChannel() const { return m_channel; }

void frc::DigitalInput::UpdateTable() {
  if (m_table != nullptr) {
    m_table->PutBoolean("Value", Get());
  }
}

void frc::DigitalInput::StartLiveWindowMode() {}

void frc::DigitalInput::StopLiveWindowMode() {}

std::string frc::DigitalInput::GetSmartDashboardType() const {
  return "DigitalInput";
}

void frc::DigitalInput::InitTable(std::shared_ptr<ITable> subTable) {
  m_table = subTable;
  UpdateTable();
}

std::shared_ptr<ITable> frc::DigitalInput::GetTable() const { return m_table; }

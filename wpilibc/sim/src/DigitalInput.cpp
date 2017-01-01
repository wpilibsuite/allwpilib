/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DigitalInput.h"

#include <sstream>

#include "WPIErrors.h"

using namespace frc;

/**
 * Create an instance of a Digital Input class.
 * Creates a digital input given a channel and uses the default module.
 *
 * @param channel The digital channel (1..14).
 */
DigitalInput::DigitalInput(int channel) : m_channel(channel) {
  std::stringstream ss;
  ss << "dio/" << channel;
  m_impl = new SimDigitalInput(ss.str());
}

/**
 * Get the value from a digital input channel.
 * Retrieve the value of a single digital input channel from the FPGA.
 */
int DigitalInput::Get() const { return m_impl->Get(); }

/**
 * @return The GPIO channel number that this object represents.
 */
int DigitalInput::GetChannel() const { return m_channel; }

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

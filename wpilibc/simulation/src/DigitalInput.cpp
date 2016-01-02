/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DigitalInput.h"
#include "WPIErrors.h"

/**
 * Create an instance of a Digital Input class.
 * Creates a digital input given a channel and uses the default module.
 *
 * @param channel The digital channel (1..14).
 */
DigitalInput::DigitalInput(uint32_t channel)
{
	char buf[64];
	m_channel = channel;
	int n = sprintf(buf, "dio/%d", channel);
	m_impl = new SimDigitalInput(buf);
}

/*
 * Get the value from a digital input channel.
 * Retrieve the value of a single digital input channel from the FPGA.
 */
uint32_t DigitalInput::Get() const
{
	return m_impl->Get();
}

/**
 * @return The GPIO channel number that this object represents.
 */
uint32_t DigitalInput::GetChannel() const
{
	return m_channel;
}

void DigitalInput::UpdateTable() {
	if (m_table != nullptr) {
		m_table->PutBoolean("Value", Get());
	}
}

void DigitalInput::StartLiveWindowMode() {

}

void DigitalInput::StopLiveWindowMode() {

}

std::string DigitalInput::GetSmartDashboardType() const {
	return "DigitalInput";
}

void DigitalInput::InitTable(std::shared_ptr<ITable> subTable) {
	m_table = subTable;
	UpdateTable();
}

std::shared_ptr<ITable> DigitalInput::GetTable() const {
	return m_table;
}

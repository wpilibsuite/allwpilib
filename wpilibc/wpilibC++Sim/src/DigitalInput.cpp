/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "DigitalInput.h"
#include "WPIErrors.h"

/**
 * Create an instance of a DigitalInput.
 * Creates a digital input given a channel. Common creation routine for all
 * constructors.
 */
void DigitalInput::InitDigitalInput(uint32_t channel)
{
	m_table = NULL;
	char buf[64];
	m_channel = channel;
	int n = sprintf(buf, "dio/%d", channel);
	m_impl = new SimDigitalInput(buf);
}

/**
 * Create an instance of a Digital Input class.
 * Creates a digital input given a channel and uses the default module.
 *
 * @param channel The digital channel (1..14).
 */
DigitalInput::DigitalInput(uint32_t channel)
{
	InitDigitalInput(channel);
}

/**
 * Free resources associated with the Digital Input class.
 */
DigitalInput::~DigitalInput()
{
}

/*
 * Get the value from a digital input channel.
 * Retrieve the value of a single digital input channel from the FPGA.
 */
uint32_t DigitalInput::Get()
{
	return m_impl->Get();
}

/**
 * @return The GPIO channel number that this object represents.
 */
uint32_t DigitalInput::GetChannel()
{
	return m_channel;
}

void DigitalInput::UpdateTable() {
	if (m_table != NULL) {
		m_table->PutBoolean("Value", Get());
	}
}

void DigitalInput::StartLiveWindowMode() {

}

void DigitalInput::StopLiveWindowMode() {

}

std::string DigitalInput::GetSmartDashboardType() {
	return "DigitalInput";
}

void DigitalInput::InitTable(ITable *subTable) {
	m_table = subTable;
	UpdateTable();
}

ITable * DigitalInput::GetTable() {
	return m_table;
}

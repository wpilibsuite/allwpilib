/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "AnalogChannel.h"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"

/**
 * Common initialization.
 */
void AnalogChannel::InitChannel(uint8_t moduleNumber, uint32_t channel)
{
	m_table = NULL;
    m_module = moduleNumber;
    m_channel = channel;
    char buffer[50];
    int n = sprintf(buffer, "analog/%d/%d", moduleNumber, channel);
    m_impl = new SimFloatInput(buffer);

	LiveWindow::GetInstance()->AddSensor("AnalogChannel",channel, GetModuleNumber(), this);
}

/**
 * Construct an analog channel on a specified module.
 * 
 * @param moduleNumber The analog module (1 or 2).
 * @param channel The channel number to represent.
 */
AnalogChannel::AnalogChannel(uint8_t moduleNumber, uint32_t channel)
{
	InitChannel(moduleNumber, channel);
}

/**
 * Construct an analog channel on the default module.
 * 
 * @param channel The channel number to represent.
 */
AnalogChannel::AnalogChannel(uint32_t channel)
{
	InitChannel(GetDefaultAnalogModule(), channel);
}

/**
 * Channel destructor.
 */
AnalogChannel::~AnalogChannel()
{
}

/**
 * Get a scaled sample straight from this channel on the module.
 * The value is scaled to units of Volts using the calibrated scaling data from GetLSBWeight() and GetOffset().
 * @return A scaled sample straight from this channel on the module.
 */
float AnalogChannel::GetVoltage()
{
    return m_impl->Get();
}

/**
 * Get a scaled sample from the output of the oversample and average engine for this channel.
 * The value is scaled to units of Volts using the calibrated scaling data from GetLSBWeight() and GetOffset().
 * Using oversampling will cause this value to be higher resolution, but it will update more slowly.
 * Using averaging will cause this value to be more stable, but it will update more slowly.
 * @return A scaled sample from the output of the oversample and average engine for this channel.
 */
float AnalogChannel::GetAverageVoltage()
{
    return m_impl->Get();
}

/**
 * Get the channel number.
 * @return The channel number.
 */
uint32_t AnalogChannel::GetChannel()
{
	return m_channel;
}

/**
 * Get the module number.
 * @return The module number.
 */
uint8_t AnalogChannel::GetModuleNumber()
{
	return m_module;
}

/**
 * Get the Average value for the PID Source base object.
 * 
 * @return The average voltage.
 */
double AnalogChannel::PIDGet() 
{
	return GetAverageVoltage();
}

void AnalogChannel::UpdateTable() {
	if (m_table != NULL) {
		m_table->PutNumber("Value", GetAverageVoltage());
	}
}

void AnalogChannel::StartLiveWindowMode() {
	
}

void AnalogChannel::StopLiveWindowMode() {
	
}

std::string AnalogChannel::GetSmartDashboardType() {
	return "Analog Input";
}

void AnalogChannel::InitTable(ITable *subTable) {
	m_table = subTable;
	UpdateTable();
}

ITable * AnalogChannel::GetTable() {
	return m_table;
}



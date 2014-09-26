/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Solenoid.h"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"
#include "simulation/simTime.h"

/**
 * Common function to implement constructor behavior.
 */
void Solenoid::InitSolenoid(int slot, int channel)
{
    char buffer[50];
    int n = sprintf(buffer, "pneumatic/%d/%d", slot, channel);
    m_impl = new SimContinuousOutput(buffer);
  
	LiveWindow::GetInstance()->AddActuator("Solenoid", slot, channel, this);
}

/**
 * Constructor.
 *
 * @param channel The channel on the solenoid module to control (1..8).
 */
Solenoid::Solenoid(uint32_t channel)
{
    InitSolenoid(1, channel);
}

/**
 * Constructor.
 *
 * @param moduleNumber The solenoid module (1 or 2).
 * @param channel The channel on the solenoid module to control (1..8).
 */
Solenoid::Solenoid(uint8_t moduleNumber, uint32_t channel)
{
    InitSolenoid(moduleNumber, channel);
}

/**
 * Destructor.
 */
Solenoid::~Solenoid()
{
}

/**
 * Set the value of a solenoid.
 *
 * @param on Turn the solenoid output off or on.
 */
void Solenoid::Set(bool on)
{
    m_on = on;
    m_impl->Set(on ? 1 : -1);
}

/**
 * Read the current value of the solenoid.
 *
 * @return The current value of the solenoid.
 */
bool Solenoid::Get()
{
    return m_on;
}


void Solenoid::ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew) {
	Set(value.b);
}

void Solenoid::UpdateTable() {
	if (m_table != NULL) {
		m_table->PutBoolean("Value", Get());
	}
}

void Solenoid::StartLiveWindowMode() {
	Set(false);
	if (m_table != NULL) {
		m_table->AddTableListener("Value", this, true);
	}
}

void Solenoid::StopLiveWindowMode() {
	Set(false);
	if (m_table != NULL) {
		m_table->RemoveTableListener(this);
	}
}

std::string Solenoid::GetSmartDashboardType() {
	return "Solenoid";
}

void Solenoid::InitTable(ITable *subTable) {
	m_table = subTable;
	UpdateTable();
}

ITable * Solenoid::GetTable() {
	return m_table;
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Solenoid.h"
//#include "NetworkCommunication/UsageReporting.h"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"

/**
 * Common function to implement constructor behavior.
 */
void Solenoid::InitSolenoid()
{
	m_table = NULL;
	char buf[64];
	if (!CheckSolenoidModule(m_moduleNumber))
	{
		snprintf(buf, 64, "Solenoid Module %d", m_moduleNumber);
		wpi_setWPIErrorWithContext(ModuleIndexOutOfRange, buf);
		return;
	}
	if (!CheckSolenoidChannel(m_channel))
	{
		snprintf(buf, 64, "Solenoid Channel %d", m_channel);
		wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf);
		return;
	}
	Resource::CreateResourceObject(&m_allocated, kSolenoidChannels * 63);

	snprintf(buf, 64, "Solenoid %d (Module: %d)", m_channel, m_moduleNumber);
	if (m_allocated->Allocate(m_moduleNumber * kSolenoidChannels + m_channel, buf) == ~0ul)
	{
		CloneError(m_allocated);
		return;
	}

	LiveWindow::GetInstance()->AddActuator("Solenoid", m_moduleNumber, m_channel, this);
	HALReport(HALUsageReporting::kResourceType_Solenoid, m_channel, m_moduleNumber);
}

/**
 * Constructor using the default PCM ID (0).
 *
 * @param channel The channel on the PCM to control (0..7).
 */
Solenoid::Solenoid(uint32_t channel)
	: SolenoidBase (GetDefaultSolenoidModule())
	, m_channel (channel)
{
	InitSolenoid();
}

/**
 * Constructor.
 *
 * @param moduleNumber The CAN ID of the PCM the solenoid is attached to
 * @param channel The channel on the PCM to control (0..7).
 */
Solenoid::Solenoid(uint8_t moduleNumber, uint32_t channel)
	: SolenoidBase (moduleNumber)
	, m_channel (channel)
{
	InitSolenoid();
}

/**
 * Destructor.
 */
Solenoid::~Solenoid()
{
	if (CheckSolenoidModule(m_moduleNumber))
	{
		m_allocated->Free(m_moduleNumber * kSolenoidChannels + m_channel);
	}
}

/**
 * Set the value of a solenoid.
 *
 * @param on Turn the solenoid output off or on.
 */
void Solenoid::Set(bool on)
{
	if (StatusIsFatal()) return;
	uint8_t value = on ? 0xFF : 0x00;
	uint8_t mask = 1 << m_channel;

	SolenoidBase::Set(value, mask);
}

/**
 * Read the current value of the solenoid.
 *
 * @return The current value of the solenoid.
 */
bool Solenoid::Get()
{
	if (StatusIsFatal()) return false;
	uint8_t value = GetAll() & ( 1 << m_channel);
	return (value != 0);
}
/**
 * Check if solenoid is blacklisted.
 *		If a solenoid is shorted, it is added to the blacklist and
 *		disabled until power cycle, or until faults are cleared.
 *		@see ClearAllPCMStickyFaults()
 *
 * @return If solenoid is disabled due to short.
 */
bool Solenoid::IsBlackListed()
{
	int value = GetPCMSolenoidBlackList() & ( 1 << m_channel);
	return (value != 0);
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

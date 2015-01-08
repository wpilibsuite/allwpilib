/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "DoubleSolenoid.h"
//#include "NetworkCommunication/UsageReporting.h"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"

/**
 * Common function to implement constructor behaviour.
 */
void DoubleSolenoid::InitSolenoid()
{
	m_table = NULL;
	char buf[64];
	if (!CheckSolenoidModule(m_moduleNumber))
	{
		snprintf(buf, 64, "Solenoid Module %d", m_moduleNumber);
		wpi_setWPIErrorWithContext(ModuleIndexOutOfRange, buf);
		return;
	}
	if (!CheckSolenoidChannel(m_forwardChannel))
	{
		snprintf(buf, 64, "Solenoid Channel %d", m_forwardChannel);
		wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf);
		return;
	}
	if (!CheckSolenoidChannel(m_reverseChannel))
	{
		snprintf(buf, 64, "Solenoid Channel %d", m_reverseChannel);
		wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf);
		return;
	}
	Resource::CreateResourceObject(&m_allocated, solenoid_kNumDO7_0Elements * kSolenoidChannels);

	snprintf(buf, 64, "Solenoid %d (Module: %d)", m_forwardChannel, m_moduleNumber);
	if (m_allocated->Allocate(m_moduleNumber * kSolenoidChannels + m_forwardChannel, buf) == ~0ul)
	{
		CloneError(m_allocated);
		return;
	}

	snprintf(buf, 64, "Solenoid %d (Module: %d)", m_reverseChannel, m_moduleNumber);
	if (m_allocated->Allocate(m_moduleNumber * kSolenoidChannels + m_reverseChannel, buf) == ~0ul)
	{
		CloneError(m_allocated);
		return;
	}
    
	m_forwardMask = 1 << m_forwardChannel;
	m_reverseMask = 1 << m_reverseChannel;
	HALReport(HALUsageReporting::kResourceType_Solenoid, m_forwardChannel, m_moduleNumber);
	HALReport(HALUsageReporting::kResourceType_Solenoid, m_reverseChannel, m_moduleNumber);
	LiveWindow::GetInstance()->AddActuator("DoubleSolenoid", m_moduleNumber, m_forwardChannel, this);
}

/**
 * Constructor.
 * Uses the default PCM ID of 0
 * @param forwardChannel The forward channel number on the PCM (0..7).
 * @param reverseChannel The reverse channel number on the PCM (0..7).
 */
DoubleSolenoid::DoubleSolenoid(uint32_t forwardChannel, uint32_t reverseChannel)
	: SolenoidBase (GetDefaultSolenoidModule())
	, m_forwardChannel (forwardChannel)
	, m_reverseChannel (reverseChannel)
{
	InitSolenoid();
}

/**
 * Constructor.
 *
 * @param moduleNumber The CAN ID of the PCM.
 * @param forwardChannel The forward channel on the PCM to control (0..7).
 * @param reverseChannel The reverse channel on the PCM to control (0..7).
 */
DoubleSolenoid::DoubleSolenoid(uint8_t moduleNumber, uint32_t forwardChannel, uint32_t reverseChannel)
	: SolenoidBase (moduleNumber)
	, m_forwardChannel (forwardChannel)
	, m_reverseChannel (reverseChannel)
{
	InitSolenoid();
}

/**
 * Destructor.
 */
DoubleSolenoid::~DoubleSolenoid()
{
	if (CheckSolenoidModule(m_moduleNumber))
	{
		m_allocated->Free(m_moduleNumber * kSolenoidChannels + m_forwardChannel);
		m_allocated->Free(m_moduleNumber * kSolenoidChannels + m_reverseChannel);
	}
}

/**
 * Set the value of a solenoid.
 *
 * @param value The value to set (Off, Forward or Reverse)
 */
void DoubleSolenoid::Set(Value value)
{
	if (StatusIsFatal()) return;
	uint8_t rawValue = 0x00;

	switch(value)
	{
	case kOff:
		rawValue = 0x00;
		break;
	case kForward:
		rawValue = m_forwardMask;
		break;
	case kReverse:
		rawValue = m_reverseMask;
		break;
	}

	SolenoidBase::Set(rawValue, m_forwardMask | m_reverseMask);
}

/**
 * Read the current value of the solenoid.
 *
 * @return The current value of the solenoid.
 */
DoubleSolenoid::Value DoubleSolenoid::Get()
{
	if (StatusIsFatal()) return kOff;
	uint8_t value = GetAll();

	if (value & m_forwardMask) return kForward;
	if (value & m_reverseMask) return kReverse;
	return kOff;
}
/**
 * Check if the forward solenoid is blacklisted.
 *		If a solenoid is shorted, it is added to the blacklist and
 *		disabled until power cycle, or until faults are cleared.
 *		@see ClearAllPCMStickyFaults()
 *
 * @return If solenoid is disabled due to short.
 */
bool DoubleSolenoid::IsFwdSolenoidBlackListed()
{
	int blackList = GetPCMSolenoidBlackList();
	return (blackList & m_forwardMask) ? 1 : 0;
}
/**
 * Check if the reverse solenoid is blacklisted.
 *		If a solenoid is shorted, it is added to the blacklist and
 *		disabled until power cycle, or until faults are cleared.
 *		@see ClearAllPCMStickyFaults()
 *
 * @return If solenoid is disabled due to short.
 */
bool DoubleSolenoid::IsRevSolenoidBlackListed()
{
	int blackList = GetPCMSolenoidBlackList();
	return (blackList & m_reverseMask) ? 1 : 0;
}

void DoubleSolenoid::ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew) {
	Value lvalue = kOff;
	std::string *val = (std::string *)value.ptr;
	if (*val == "Forward")
		lvalue = kForward;
	else if (*val == "Reverse")
		lvalue = kReverse;
	Set(lvalue);
}

void DoubleSolenoid::UpdateTable() {
	if (m_table != NULL) {
		m_table->PutString("Value", (Get() == kForward ? "Forward" : (Get() == kReverse ? "Reverse" : "Off")));
	}
}

void DoubleSolenoid::StartLiveWindowMode() {
	Set(kOff);
	if (m_table != NULL) {
		m_table->AddTableListener("Value", this, true);
	}
}

void DoubleSolenoid::StopLiveWindowMode() {
	Set(kOff);
	if (m_table != NULL) {
		m_table->RemoveTableListener(this);
	}
}

std::string DoubleSolenoid::GetSmartDashboardType() {
	return "Double Solenoid";
}

void DoubleSolenoid::InitTable(ITable *subTable) {
	m_table = subTable;
	UpdateTable();
}

ITable * DoubleSolenoid::GetTable() {
	return m_table;
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Relay.h"

#include "DigitalModule.h"
#include "NetworkCommunication/UsageReporting.h"
#include "Resource.h"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"

// Allocate each direction separately.
static Resource *relayChannels = NULL;

/**
 * Common relay intitialization methode.
 * This code is common to all Relay constructors and initializes the relay and reserves
 * all resources that need to be locked. Initially the relay is set to both lines at 0v.
 * @param slot The module slot number this relay is connected to.
 * 
 * @param moduleNumber The digital module this relay is connected to (1 or 2).
 */
void Relay::InitRelay (uint8_t moduleNumber)
{
	m_table = NULL;
	char buf[64];
	Resource::CreateResourceObject(&relayChannels, dio_kNumSystems * kRelayChannels * 2);
	if (!SensorBase::CheckRelayModule(moduleNumber))
	{
		snprintf(buf, 64, "Digital Module %d", moduleNumber);
		wpi_setWPIErrorWithContext(ModuleIndexOutOfRange, buf);
		return;
	}
	if (!SensorBase::CheckRelayChannel(m_channel))
	{
		snprintf(buf, 64, "Relay Channel %d", m_channel);
		wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf);
		return;
	}

	if (m_direction == kBothDirections || m_direction == kForwardOnly)
	{
		snprintf(buf, 64, "Forward Relay %d (Module: %d)", m_channel, moduleNumber);
		if (relayChannels->Allocate(((moduleNumber - 1) * kRelayChannels + m_channel - 1) * 2, buf) == ~0ul)
		{
			CloneError(relayChannels);
			return;
		}

		nUsageReporting::report(nUsageReporting::kResourceType_Relay, m_channel, moduleNumber - 1);
	}
	if (m_direction == kBothDirections || m_direction == kReverseOnly)
	{
		snprintf(buf, 64, "Reverse Relay %d (Module: %d)", m_channel, moduleNumber);
		if (relayChannels->Allocate(((moduleNumber - 1) * kRelayChannels + m_channel - 1) * 2 + 1, buf) == ~0ul)
		{
			CloneError(relayChannels);
			return;
		}

		nUsageReporting::report(nUsageReporting::kResourceType_Relay, m_channel + 128, moduleNumber - 1);
	}
	m_module = DigitalModule::GetInstance(moduleNumber);
	m_module->SetRelayForward(m_channel, false);
	m_module->SetRelayReverse(m_channel, false);
	LiveWindow::GetInstance()->AddActuator("Relay", moduleNumber, m_channel, this);
}

/**
 * Relay constructor given the module and the channel.
 * 
 * @param moduleNumber The digital module this relay is connected to (1 or 2).
 * @param channel The channel number within the module for this relay.
 * @param direction The direction that the Relay object will control.
 */
Relay::Relay(uint8_t moduleNumber, uint32_t channel, Relay::Direction direction)
	: m_channel (channel)
	, m_direction (direction)
{
	InitRelay(moduleNumber);
}

/**
 * Relay constructor given a channel only where the default digital module is used.
 * @param channel The channel number within the default module for this relay.
 * @param direction The direction that the Relay object will control.
 */
Relay::Relay(uint32_t channel, Relay::Direction direction)
	: m_channel (channel)
	, m_direction (direction)
{
	InitRelay(GetDefaultDigitalModule());
}

/**
 * Free the resource associated with a relay.
 * The relay channels are set to free and the relay output is turned off.
 */
Relay::~Relay()
{
	m_module->SetRelayForward(m_channel, false);
	m_module->SetRelayReverse(m_channel, false);

	if (m_direction == kBothDirections || m_direction == kForwardOnly)
	{
		relayChannels->Free(((m_module->GetNumber() - 1) * kRelayChannels + m_channel - 1) * 2);
	}
	if (m_direction == kBothDirections || m_direction == kReverseOnly)
	{
		relayChannels->Free(((m_module->GetNumber() - 1) * kRelayChannels + m_channel - 1) * 2 + 1);
	}
}

/**
 * Set the relay state.
 * 
 * Valid values depend on which directions of the relay are controlled by the object.
 * 
 * When set to kBothDirections, the relay can be any of the four states:
 *    0v-0v, 0v-12v, 12v-0v, 12v-12v
 * 
 * When set to kForwardOnly or kReverseOnly, you can specify the constant for the
 *    direction or you can simply specify kOff and kOn.  Using only kOff and kOn is
 *    recommended.
 * 
 * @param value The state to set the relay.
 */
void Relay::Set(Relay::Value value)
{
	if (StatusIsFatal()) return;
	switch (value)
	{
	case kOff:
		if (m_direction == kBothDirections || m_direction == kForwardOnly)
		{
			m_module->SetRelayForward(m_channel, false);
		}
		if (m_direction == kBothDirections || m_direction == kReverseOnly)
		{
			m_module->SetRelayReverse(m_channel, false);
		}
		break;
	case kOn:
		if (m_direction == kBothDirections || m_direction == kForwardOnly)
		{
			m_module->SetRelayForward(m_channel, true);
		}
		if (m_direction == kBothDirections || m_direction == kReverseOnly)
		{
			m_module->SetRelayReverse(m_channel, true);
		}
		break;
	case kForward:
		if (m_direction == kReverseOnly)
		{
			wpi_setWPIError(IncompatibleMode);
			break;
		}
		if (m_direction == kBothDirections || m_direction == kForwardOnly)
		{
			m_module->SetRelayForward(m_channel, true);
		}
		if (m_direction == kBothDirections)
		{
			m_module->SetRelayReverse(m_channel, false);
		}
		break;
	case kReverse:
		if (m_direction == kForwardOnly)
		{
			wpi_setWPIError(IncompatibleMode);
			break;
		}
		if (m_direction == kBothDirections)
		{
			m_module->SetRelayForward(m_channel, false);
		}
		if (m_direction == kBothDirections || m_direction == kReverseOnly)
		{
			m_module->SetRelayReverse(m_channel, true);
		}
		break;
	}
}

/**
 * Get the Relay State
 * 
 * Gets the current state of the relay.
 * 
 * When set to kForwardOnly or kReverseOnly, value is returned as kOn/kOff not
 * kForward/kReverse (per the recommendation in Set)
 * 
 * @return The current state of the relay as a Relay::Value
 */
Relay::Value Relay::Get() {
   if(m_module->GetRelayForward(m_channel)) {
	   if(m_module->GetRelayReverse(m_channel)) {
		   return kOn;
	   } else {
		   if(m_direction == kForwardOnly) {
			   return kOn;
		   } else {
		   return kForward;
		   }
	   }
   } else {
	   if(m_module->GetRelayReverse(m_channel)) {
		   if(m_direction == kReverseOnly) {
			   return kOn;
		   } else {
		   return kReverse;
		   }
	   } else {
		   return kOff;
	   }
   }        
}

void Relay::ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew) {
	std::string *val = (std::string *) value.ptr;
	if (*val == "Off") Set(kOff);
	else if (*val == "Forward") Set(kForward);
	else if (*val == "Reverse") Set(kReverse);
}

void Relay::UpdateTable() {
	if(m_table != NULL){
		if (Get() == kOn) {
			m_table->PutString("Value", "On");
		}
		else if (Get() == kForward) {
			m_table->PutString("Value", "Forward");
		}
		else if (Get() == kReverse) {
			m_table->PutString("Value", "Reverse");
		}
		else {
			m_table->PutString("Value", "Off");
		}
	}
}

void Relay::StartLiveWindowMode() {
	if(m_table != NULL){
		m_table->AddTableListener("Value", this, true);
	}
}

void Relay::StopLiveWindowMode() {
	if(m_table != NULL){
		m_table->RemoveTableListener(this);
	}
}

std::string Relay::GetSmartDashboardType() {
	return "Relay";
}

void Relay::InitTable(ITable *subTable) {
	m_table = subTable;
	UpdateTable();
}

ITable * Relay::GetTable() {
	return m_table;
}



/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code	*/
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Relay.h"

//#include "NetworkCommunication/UsageReporting.h"
#include "Resource.h"
#include "WPIErrors.h"
#include "LiveWindow/LiveWindow.h"
#include "HAL/HAL.hpp"

// Allocate each direction separately.
static Resource *relayChannels = NULL;

/**
 * Common relay initialization method.
 * This code is common to all Relay constructors and initializes the relay and reserves
 * all resources that need to be locked. Initially the relay is set to both lines at 0v.
 */
void Relay::InitRelay()
{
	m_table = NULL;
	char buf[64];
	Resource::CreateResourceObject(&relayChannels, dio_kNumSystems * kRelayChannels * 2);
	if (!SensorBase::CheckRelayChannel(m_channel))
	{
		snprintf(buf, 64, "Relay Channel %d", m_channel);
		wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf);
		return;
	}

	if (m_direction == kBothDirections || m_direction == kForwardOnly)
	{
		snprintf(buf, 64, "Forward Relay %d", m_channel);
		if (relayChannels->Allocate(m_channel * 2, buf) == ~0ul)
		{
			CloneError(relayChannels);
			return;
		}

		HALReport(HALUsageReporting::kResourceType_Relay, m_channel);
	}
	if (m_direction == kBothDirections || m_direction == kReverseOnly)
	{
		snprintf(buf, 64, "Reverse Relay %d", m_channel);
		if (relayChannels->Allocate(m_channel * 2 + 1, buf) == ~0ul)
		{
			CloneError(relayChannels);
			return;
		}

		HALReport(HALUsageReporting::kResourceType_Relay, m_channel + 128);
	}

	int32_t status = 0;
	setRelayForward(m_relay_ports[m_channel], false, &status);
	setRelayReverse(m_relay_ports[m_channel], false, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));

	LiveWindow::GetInstance()->AddActuator("Relay", 1, m_channel, this);
}

/**
 * Relay constructor given a channel.
 * @param channel The channel number (0-3).
 * @param direction The direction that the Relay object will control.
 */
Relay::Relay(uint32_t channel, Relay::Direction direction)
	: m_channel (channel)
	, m_direction (direction)
{
	InitRelay();
}

/**
 * Free the resource associated with a relay.
 * The relay channels are set to free and the relay output is turned off.
 */
Relay::~Relay()
{

	int32_t status = 0;
	setRelayForward(m_relay_ports[m_channel], false, &status);
	setRelayReverse(m_relay_ports[m_channel], false, &status);
	wpi_setErrorWithContext(status, getHALErrorMessage(status));

	if (m_direction == kBothDirections || m_direction == kForwardOnly)
	{
		relayChannels->Free(m_channel * 2);
	}
	if (m_direction == kBothDirections || m_direction == kReverseOnly)
	{
		relayChannels->Free(m_channel * 2 + 1);
	}
}

/**
 * Set the relay state.
 *
 * Valid values depend on which directions of the relay are controlled by the object.
 *
 * When set to kBothDirections, the relay can be any of the four states:
 *	 0v-0v, 0v-12v, 12v-0v, 12v-12v
 *
 * When set to kForwardOnly or kReverseOnly, you can specify the constant for the
 *	 direction or you can simply specify kOff and kOn.  Using only kOff and kOn is
 *	 recommended.
 *
 * @param value The state to set the relay.
 */
void Relay::Set(Relay::Value value)
{
	if (StatusIsFatal()) return;

	int32_t status = 0;

	switch (value)
	{
	case kOff:
		if (m_direction == kBothDirections || m_direction == kForwardOnly)
		{
			setRelayForward(m_relay_ports[m_channel], false, &status);
		}
		if (m_direction == kBothDirections || m_direction == kReverseOnly)
		{
			setRelayReverse(m_relay_ports[m_channel], false, &status);
		}
		break;
	case kOn:
		if (m_direction == kBothDirections || m_direction == kForwardOnly)
		{
			setRelayForward(m_relay_ports[m_channel], true, &status);
		}
		if (m_direction == kBothDirections || m_direction == kReverseOnly)
		{
			setRelayReverse(m_relay_ports[m_channel], true, &status);
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
			setRelayForward(m_relay_ports[m_channel], true, &status);
		}
		if (m_direction == kBothDirections)
		{
			setRelayReverse(m_relay_ports[m_channel], false, &status);
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
			setRelayForward(m_relay_ports[m_channel], false, &status);
		}
		if (m_direction == kBothDirections || m_direction == kReverseOnly)
		{
			setRelayReverse(m_relay_ports[m_channel], true, &status);
		}
		break;
	}

	wpi_setErrorWithContext(status, getHALErrorMessage(status));
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
	int32_t status;

	if(getRelayForward(m_relay_ports[m_channel], &status)) {
		if(getRelayReverse(m_relay_ports[m_channel], &status)) {
			return kOn;
		} else {
			if(m_direction == kForwardOnly) {
				return kOn;
			} else {
			return kForward;
			}
		}
	} else {
		if(getRelayReverse(m_relay_ports[m_channel], &status)) {
			if(m_direction == kReverseOnly) {
				return kOn;
			} else {
			return kReverse;
			}
		} else {
			return kOff;
		}
	}

	wpi_setErrorWithContext(status, getHALErrorMessage(status));
}

void Relay::ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew) {
	std::string *val = (std::string *) value.ptr;
	if (*val == "Off") Set(kOff);
        else if (*val == "On") Set(kOn);
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

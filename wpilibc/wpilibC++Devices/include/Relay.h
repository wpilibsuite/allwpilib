/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "SensorBase.h"
#include "tables/ITableListener.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "tables/ITable.h"

/**
 * Class for Spike style relay outputs.
 * Relays are intended to be connected to spikes or similar relays. The relay channels controls
 * a pair of pins that are either both off, one on, the other on, or both on. This translates into
 * two spike outputs at 0v, one at 12v and one at 0v, one at 0v and the other at 12v, or two
 * spike outputs at 12V. This allows off, full forward, or full reverse control of motors without
 * variable speed.  It also allows the two channels (forward and reverse) to be used independently
 * for something that does not care about voltage polatiry (like a solenoid).
 */
class Relay : public SensorBase, public ITableListener, public LiveWindowSendable
{
public:
	enum Value
	{
		kOff,
		kOn,
		kForward,
		kReverse
	};
	enum Direction
	{
		kBothDirections,
		kForwardOnly,
		kReverseOnly
	};

	Relay(uint32_t channel, Direction direction = kBothDirections);
	virtual ~Relay();

	void Set(Value value);
	Value Get();

	void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew);
	void UpdateTable();
	void StartLiveWindowMode();
	void StopLiveWindowMode();
	std::string GetSmartDashboardType();
	void InitTable(ITable *subTable);
	ITable * GetTable();

	ITable *m_table;

private:
	void InitRelay();

	uint32_t m_channel;
	Direction m_direction;
};

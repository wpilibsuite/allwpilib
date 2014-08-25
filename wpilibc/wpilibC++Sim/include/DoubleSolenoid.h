/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "simulation/SimContinuousOutput.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "tables/ITableListener.h"

/**
 * DoubleSolenoid class for running 2 channels of high voltage Digital Output
 * (PCM).
 * 
 * The DoubleSolenoid class is typically used for pneumatics solenoids that
 * have two positions controlled by two separate channels.
 */
class DoubleSolenoid : public LiveWindowSendable, public ITableListener
{
public:
	enum Value
	{
		kOff,
		kForward,
		kReverse
	};

	explicit DoubleSolenoid(uint32_t forwardChannel, uint32_t reverseChannel);
	DoubleSolenoid(uint8_t moduleNumber, uint32_t forwardChannel, uint32_t reverseChannel);
	virtual ~DoubleSolenoid();
	virtual void Set(Value value);
	virtual Value Get();

	void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew);
	void UpdateTable();
	void StartLiveWindowMode();
	void StopLiveWindowMode();
	std::string GetSmartDashboardType();
	void InitTable(ITable *subTable);
	ITable * GetTable();

private:
    void InitSolenoid(int slot, int channel1, int channel2);
    SimContinuousOutput* m_impl;
    Value m_value;
    bool m_reversed;

	ITable *m_table;
};

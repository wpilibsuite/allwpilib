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
 * Solenoid class for running high voltage Digital Output (PCM).
 * 
 * The Solenoid class is typically used for pneumatics solenoids, but could be used
 * for any device within the current spec of the PCM.
 */
class Solenoid : public LiveWindowSendable, public ITableListener
{
public:
	explicit Solenoid(uint32_t channel);
	Solenoid(uint8_t moduleNumber, uint32_t channel);
	virtual ~Solenoid();
	virtual void Set(bool on);
	virtual bool Get();

	void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew);
	void UpdateTable();
	void StartLiveWindowMode();
	void StopLiveWindowMode();
	std::string GetSmartDashboardType();
	void InitTable(ITable *subTable);
	ITable * GetTable();

private:
	void InitSolenoid(int slot, int channel);
    SimContinuousOutput* m_impl;
    bool m_on; 

	ITable *m_table;
};

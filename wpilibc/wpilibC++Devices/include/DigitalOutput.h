/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "DigitalSource.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "tables/ITableListener.h"

/**
 * Class to write to digital outputs.
 * Write values to the digital output channels. Other devices implemented elsewhere will allocate
 * channels automatically so for those devices it shouldn't be done here.
 */
class DigitalOutput : public DigitalSource, public ITableListener, public LiveWindowSendable
{
public:
	explicit DigitalOutput(uint32_t channel);
	virtual ~DigitalOutput();
	void Set(uint32_t value);
	uint32_t GetChannel();
	void Pulse(float length);
	bool IsPulsing();
	void SetPWMRate(float rate);
	void EnablePWM(float initialDutyCycle);
	void DisablePWM();
	void UpdateDutyCycle(float dutyCycle);

	// Digital Source Interface
	virtual uint32_t GetChannelForRouting();
	virtual uint32_t GetModuleForRouting();
	virtual bool GetAnalogTriggerForRouting();

	virtual void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew);
	void UpdateTable();
	void StartLiveWindowMode();
	void StopLiveWindowMode();
	std::string GetSmartDashboardType();
	void InitTable(ITable *subTable);
	ITable * GetTable();

private:
	void InitDigitalOutput(uint32_t channel);

	uint32_t m_channel;
	void *m_pwmGenerator;

	ITable *m_table;
};

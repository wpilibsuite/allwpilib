/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "HAL/HAL.hpp"
#include "AnalogTriggerOutput.h"
#include "CounterBase.h"
#include "SensorBase.h"
#include "LiveWindow/LiveWindowSendable.h"

/**
 * Class for counting the number of ticks on a digital input channel.
 * This is a general purpose class for counting repetitive events. It can return the number
 * of counts, the period of the most recent cycle, and detect when the signal being counted
 * has stopped by supplying a maximum cycle time.
 *
 * All counters will immediately start counting - Reset() them if you need them
 * to be zeroed before use.
 */
class Counter : public SensorBase, public CounterBase, public LiveWindowSendable
{
public:

	Counter();
	explicit Counter(int32_t channel);
	explicit Counter(DigitalSource *source);
	explicit Counter(DigitalSource &source);
	explicit Counter(AnalogTrigger *trigger);
	explicit Counter(AnalogTrigger &trigger);
	Counter(EncodingType encodingType, DigitalSource *upSource, DigitalSource *downSource,
			bool inverted);
	virtual ~Counter();

	void SetUpSource(int32_t channel);
	void SetUpSource(AnalogTrigger *analogTrigger, AnalogTriggerType triggerType);
	void SetUpSource(AnalogTrigger &analogTrigger, AnalogTriggerType triggerType);
	void SetUpSource(DigitalSource *source);
	void SetUpSource(DigitalSource &source);
	void SetUpSourceEdge(bool risingEdge, bool fallingEdge);
	void ClearUpSource();

	void SetDownSource(int32_t channel);
	void SetDownSource(AnalogTrigger *analogTrigger, AnalogTriggerType triggerType);
	void SetDownSource(AnalogTrigger &analogTrigger, AnalogTriggerType triggerType);
	void SetDownSource(DigitalSource *source);
	void SetDownSource(DigitalSource &source);
	void SetDownSourceEdge(bool risingEdge, bool fallingEdge);
	void ClearDownSource();

	void SetUpDownCounterMode();
	void SetExternalDirectionMode();
	void SetSemiPeriodMode(bool highSemiPeriod);
	void SetPulseLengthMode(float threshold);

	void SetReverseDirection(bool reverseDirection);

	// CounterBase interface
	int32_t Get();
	void Reset();
	double GetPeriod();
	void SetMaxPeriod(double maxPeriod);
	void SetUpdateWhenEmpty(bool enabled);
	bool GetStopped();
	bool GetDirection();
	void SetSamplesToAverage(int samplesToAverage);
	int GetSamplesToAverage();
	uint32_t GetFPGAIndex()
	{
		return m_index;
	}

	void UpdateTable();
	void StartLiveWindowMode();
	void StopLiveWindowMode();
	virtual std::string GetSmartDashboardType();
	void InitTable(ITable *subTable);
	ITable * GetTable();
protected:
	DigitalSource *m_upSource;		///< What makes the counter count up.
	DigitalSource *m_downSource;	///< What makes the counter count down.
	void* m_counter;				///< The FPGA counter object.
private:
	void InitCounter(Mode mode = kTwoPulse);

	bool m_allocatedUpSource;		///< Was the upSource allocated locally?
	bool m_allocatedDownSource;	///< Was the downSource allocated locally?
	uint32_t m_index;					///< The index of this counter.

	ITable *m_table;
};

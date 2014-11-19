/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "HAL/HAL.hpp"
#include "AnalogTriggerOutput.h"
#include "SensorBase.h"

class AnalogInput;

class AnalogTrigger : public SensorBase
{
	friend class AnalogTriggerOutput;
public:
	explicit AnalogTrigger(int32_t channel);
	explicit AnalogTrigger(AnalogInput *channel);
	virtual ~AnalogTrigger();

	void SetLimitsVoltage(float lower, float upper);
	void SetLimitsRaw(int32_t lower, int32_t upper);
	void SetAveraged(bool useAveragedValue);
	void SetFiltered(bool useFilteredValue);
	uint32_t GetIndex();
	bool GetInWindow();
	bool GetTriggerState();
	AnalogTriggerOutput *CreateOutput(AnalogTriggerType type);

private:
	void InitTrigger(uint32_t channel);

	uint8_t m_index;
	void* m_trigger;
};

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef ANALOG_CHANNEL_H_
#define ANALOG_CHANNEL_H_

#include "simulation/SimFloatInput.h"
#include "SensorBase.h"
#include "PIDSource.h"
#include "LiveWindow/LiveWindowSendable.h"

/**
 * Analog channel class.
 * 
 * Each analog channel is read from hardware as a 12-bit number representing -10V to 10V.
 * 
 * Connected to each analog channel is an averaging and oversampling engine.  This engine accumulates
 * the specified ( by SetAverageBits() and SetOversampleBits() ) number of samples before returning a new
 * value.  This is not a sliding window average.  The only difference between the oversampled samples and
 * the averaged samples is that the oversampled samples are simply accumulated effectively increasing the
 * resolution, while the averaged samples are divided by the number of samples to retain the resolution,
 * but get more stable values.
 */
class AnalogChannel : public SensorBase, public PIDSource, public LiveWindowSendable
{
public:
	AnalogChannel(uint8_t moduleNumber, uint32_t channel);
	explicit AnalogChannel(uint32_t channel);
	virtual ~AnalogChannel();

	float GetVoltage();
	float GetAverageVoltage();

	uint8_t GetModuleNumber();
	uint32_t GetChannel();

	double PIDGet();
	
	void UpdateTable();
	void StartLiveWindowMode();
	void StopLiveWindowMode();
	std::string GetSmartDashboardType();
	void InitTable(ITable *subTable);
	ITable * GetTable();

private:
	void InitChannel(uint8_t moduleNumber, uint32_t channel);
	uint32_t m_channel, m_module;
	SimFloatInput* m_impl;
	int64_t m_accumulatorOffset;
	
	ITable *m_table;
};

#endif

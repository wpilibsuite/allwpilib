/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "simulation/SimFloatInput.h"
#include "SensorBase.h"
#include "PIDSource.h"
#include "LiveWindow/LiveWindowSendable.h"

#include <memory>

/**
 * Analog input class.
 *
 * Connected to each analog channel is an averaging and oversampling engine.  This engine accumulates
 * the specified ( by SetAverageBits() and SetOversampleBits() ) number of samples before returning a new
 * value.  This is not a sliding window average.  The only difference between the oversampled samples and
 * the averaged samples is that the oversampled samples are simply accumulated effectively increasing the
 * resolution, while the averaged samples are divided by the number of samples to retain the resolution,
 * but get more stable values.
 */
class AnalogInput : public SensorBase, public PIDSource, public LiveWindowSendable
{
public:
	static const uint8_t kAccumulatorModuleNumber = 1;
	static const uint32_t kAccumulatorNumChannels = 2;
	static const uint32_t kAccumulatorChannels[kAccumulatorNumChannels];

	explicit AnalogInput(uint32_t channel);
	virtual ~AnalogInput() = default;

	float GetVoltage() const;
	float GetAverageVoltage() const;

	uint32_t GetChannel() const;

	double PIDGet() override;

	void UpdateTable() override;
	void StartLiveWindowMode() override;
	void StopLiveWindowMode() override;
	std::string GetSmartDashboardType() const override;
	void InitTable(std::shared_ptr<ITable> subTable) override;
	std::shared_ptr<ITable> GetTable() const override;

private:
	uint32_t m_channel;
	SimFloatInput* m_impl;
	int64_t m_accumulatorOffset;

	std::shared_ptr<ITable> m_table;
};

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "simulation/SimDigitalInput.h"
#include "LiveWindow/LiveWindowSendable.h"

#include <memory>

/**
 * Class to read a digital input.
 * This class will read digital inputs and return the current value on the channel. Other devices
 * such as encoders, gear tooth sensors, etc. that are implemented elsewhere will automatically
 * allocate digital inputs and outputs as required. This class is only for devices like switches
 * etc. that aren't implemented anywhere else.
 */
class DigitalInput : public LiveWindowSendable {
public:
	explicit DigitalInput(uint32_t channel);
	virtual ~DigitalInput() = default;
	uint32_t Get() const;
	uint32_t GetChannel() const;

	void UpdateTable() override;
	void StartLiveWindowMode() override;
	void StopLiveWindowMode() override;
	std::string GetSmartDashboardType() const override;
	void InitTable(std::shared_ptr<ITable> subTable) override;
	std::shared_ptr<ITable> GetTable() const override;

private:
	uint32_t m_channel;
	bool m_lastValue;
	SimDigitalInput *m_impl;

	std::shared_ptr<ITable> m_table;
};

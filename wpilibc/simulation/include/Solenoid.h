/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "simulation/SimContinuousOutput.h"
#include "LiveWindow/LiveWindowSendable.h"
#include "tables/ITableListener.h"

#include <memory>

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
	virtual bool Get() const;

	void ValueChanged(ITable* source, llvm::StringRef key,
                    std::shared_ptr<nt::Value> value, bool isNew) override;
	void UpdateTable() override;
	void StartLiveWindowMode() override;
	void StopLiveWindowMode() override;
	std::string GetSmartDashboardType() const override;
	void InitTable(std::shared_ptr<ITable> subTable) override;
	std::shared_ptr<ITable> GetTable() const override;

private:
    SimContinuousOutput* m_impl;
    bool m_on;

	std::shared_ptr<ITable> m_table;
};

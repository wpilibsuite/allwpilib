/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "SensorBase.h"
#include "LiveWindow/LiveWindowSendable.h"

/**
 * Built-in accelerometer.
 *
 * This class allows access to the RoboRIO's internal accelerometer.
 */
class BuiltInAccelerometer : public SensorBase, public LiveWindowSendable
{
public:
	enum Range
	{
		kRange_2G = 0x00,
		kRange_4G = 0x01,
		kRange_8G = 0x02,
	};

	BuiltInAccelerometer(Range range = kRange_8G);
	virtual double GetX() const;
	virtual double GetY() const;
	virtual double GetZ() const;

	virtual std::string GetSmartDashboardType();
	virtual void InitTable(ITable *subtable);
	virtual void UpdateTable();
	virtual ITable* GetTable();
	virtual void StartLiveWindowMode() {}
	virtual void StopLiveWindowMode() {}

private:
	ITable *m_table;
};

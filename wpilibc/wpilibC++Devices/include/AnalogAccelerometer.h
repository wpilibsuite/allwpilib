/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "AnalogInput.h"
#include "SensorBase.h"
#include "PIDSource.h"
#include "LiveWindow/LiveWindowSendable.h"

/**
 * Handle operation of an analog accelerometer.
 * The accelerometer reads acceleration directly through the sensor. Many sensors have
 * multiple axis and can be treated as multiple devices. Each is calibrated by finding
 * the center value over a period of time.
 */
class AnalogAccelerometer : public SensorBase, public PIDSource, public LiveWindowSendable {
public:
	explicit AnalogAccelerometer(int32_t channel);
	explicit AnalogAccelerometer(AnalogInput *channel);
	virtual ~AnalogAccelerometer();

	float GetAcceleration();
	void SetSensitivity(float sensitivity);
	void SetZero(float zero);
	double PIDGet();

	void UpdateTable();
	void StartLiveWindowMode();
	void StopLiveWindowMode();
	std::string GetSmartDashboardType();
	void InitTable(ITable *subTable);
	ITable * GetTable();

private:
	void InitAccelerometer();

	AnalogInput *m_AnalogInput;
	float m_voltsPerG;
	float m_zeroGVoltage;
	bool m_allocatedChannel;

	ITable *m_table;
};

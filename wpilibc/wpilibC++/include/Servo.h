/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "SafePWM.h"
#include "SpeedController.h"

/**
 * Standard hobby style servo.
 *
 * The range parameters default to the appropriate values for the Hitec HS-322HD servo provided
 * in the FIRST Kit of Parts in 2008.
 */
class Servo : public SafePWM
{
public:
	explicit Servo(uint32_t channel);
	virtual ~Servo();
	void Set(float value);
	void SetOffline();
	float Get();
	void SetAngle(float angle);
	float GetAngle();
	static float GetMaxAngle()
	{
		return kMaxServoAngle;
	}
	static float GetMinAngle()
	{
		return kMinServoAngle;
	}

	void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew);
	void UpdateTable();
	void StartLiveWindowMode();
	void StopLiveWindowMode();
	std::string GetSmartDashboardType();
	void InitTable(ITable *subTable);
	ITable * GetTable();

	ITable *m_table;

private:
	void InitServo();
	float GetServoAngleRange()
	{
		return kMaxServoAngle - kMinServoAngle;
	}

	static constexpr float kMaxServoAngle = 180.0;
	static constexpr float kMinServoAngle = 0.0;

	static constexpr float kDefaultMaxServoPWM = 2.4;
	static constexpr float kDefaultMinServoPWM = .6;
};

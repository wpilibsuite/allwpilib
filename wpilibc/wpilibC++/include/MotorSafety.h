/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#define DEFAULT_SAFETY_EXPIRATION 0.1

class MotorSafety
{
public:
	virtual void SetExpiration(float timeout) = 0;
	virtual float GetExpiration() = 0;
	virtual bool IsAlive() = 0;
	virtual void StopMotor() = 0;
	virtual void SetSafetyEnabled(bool enabled) = 0;
	virtual bool IsSafetyEnabled() = 0;
	virtual void GetDescription(char *desc) = 0;
};

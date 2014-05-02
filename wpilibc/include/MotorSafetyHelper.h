/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "ErrorBase.h"
#include "HAL/cpp/Synchronized.hpp"

class MotorSafety;

class MotorSafetyHelper : public ErrorBase
{
public:
	MotorSafetyHelper(MotorSafety *safeObject);
	~MotorSafetyHelper();
	void Feed();
	void SetExpiration(float expirationTime);
	float GetExpiration();
	bool IsAlive();
	void Check();
	void SetSafetyEnabled(bool enabled);
	bool IsSafetyEnabled();
	static void CheckMotors();
private:
	double m_expiration;			// the expiration time for this object
	bool m_enabled;					// true if motor safety is enabled for this motor
	double m_stopTime; 				// the FPGA clock value when this motor has expired
	ReentrantSemaphore m_syncMutex;			// protect accesses to the state for this object
	MotorSafety *m_safeObject;		// the object that is using the helper
	MotorSafetyHelper *m_nextHelper; // next object in the list of MotorSafetyHelpers
	static MotorSafetyHelper *m_headHelper; // the head of the list of MotorSafetyHelper objects
	static ReentrantSemaphore m_listMutex;	// protect accesses to the list of helpers
};

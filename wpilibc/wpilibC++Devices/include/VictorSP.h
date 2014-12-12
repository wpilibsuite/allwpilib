/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "SafePWM.h"
#include "SpeedController.h"
#include "PIDOutput.h"

/**
 * Vex Robotics Victor SP Speed Controller
 */
class VictorSP : public SafePWM, public SpeedController
{
public:
	explicit VictorSP(uint32_t channel);
	virtual ~VictorSP();
	virtual void Set(float value, uint8_t syncGroup = 0);
	virtual float Get();
	virtual void Disable();

	virtual void PIDWrite(float output);

private:
	void InitVictorSP();
};

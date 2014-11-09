/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "SafePWM.h"
#include "SpeedController.h"
#include "PIDOutput.h"

class CanTalonSRX;

/**
 * CTRE Talon SRX Speed Controller
 */
class CANTalon : public MotorSafety,
                 public SpeedController,
                 public ErrorBase
{
public:
	explicit CANTalon(uint8_t deviceNumber);
	virtual ~CANTalon();
	virtual void Set(float value, uint8_t syncGroup = 0) override;
	virtual float Get() override;
	virtual void Disable() override;

private:
	CanTalonSRX *m_impl;
};

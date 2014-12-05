/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "Talon.h"

/**
 * Cross the Road Electronics (CTRE) Talon SRX Speed Controller with PWM control
 * @see CANTalon for CAN control
 */
class TalonSRX: public Talon {
public:
	explicit TalonSRX(uint32_t channel);
	virtual ~TalonSRX();
};

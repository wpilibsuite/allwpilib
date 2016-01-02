/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "SafePWM.h"
#include "SpeedController.h"
#include "PIDOutput.h"

/**
 * CTRE Talon Speed Controller
 */
class Talon : public SafePWM, public SpeedController
{
public:
	explicit Talon(uint32_t channel);
	virtual ~Talon() = default;
	virtual void Set(float value, uint8_t syncGroup = 0);
	virtual float Get() const;
	virtual void Disable();

	virtual void PIDWrite(float output) override;
};

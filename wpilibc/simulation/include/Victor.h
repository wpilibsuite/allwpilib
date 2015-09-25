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
 * IFI Victor Speed Controller
 */
class Victor : public SafePWM, public SpeedController
{
public:
	explicit Victor(uint32_t channel);
	virtual ~Victor() = default;
	virtual void Set(float value, uint8_t syncGroup = 0);
	virtual float Get() const;
	virtual void Disable();

	virtual void PIDWrite(float output) override;
};

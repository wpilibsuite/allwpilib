/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "Talon.h"

/**
 * Vex Robotics Victor SP Speed Controller
 */
class VictorSP: public Talon {
public:
	explicit VictorSP(uint32_t channel);
	virtual ~VictorSP();
};

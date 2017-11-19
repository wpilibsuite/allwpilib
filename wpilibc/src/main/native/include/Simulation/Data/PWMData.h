/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <cinttypes>

namespace frc {
namespace sim {

	/** 
	 * PWM Data structure, containing data for each of the PWM Channels.
	 * This is intended to be used in a Model instance.
	 */
	typedef struct PWMData_ {
		bool initialized;
		int32_t raw;
		double speed;
		double position;
		int32_t periodScale;
		bool zeroLatch;
	} PWMData;
	
	/**
	 * Get the number of available PWM Channels (both onboard + MXP)
	 */
	int GetNumPWM();

	/**
	 * Get PWM Data for a specific channel. This only works in simulation.
	 */
	PWMData GetPWMData(int index);

} // namespace sim
} // namespace frc
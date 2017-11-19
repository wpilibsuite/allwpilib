/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Simulation/Data/PWMData.h"
#include "HAL/HAL.h"
#include "MockData/PWMData.h"

int frc::sim::GetNumPWM() {
	return HAL_GetNumPWMChannels();
}

frc::sim::PWMData frc::sim::GetPWMData(int index) {
#ifndef CONFIG_ATHENA
	PWMData data = {
		HALSIM_GetPWMInitialized(index),		// initialized
		HALSIM_GetPWMRawValue(index),			// raw
		HALSIM_GetPWMSpeed(index),				// speed
		HALSIM_GetPWMPosition(index),			// position
		HALSIM_GetPWMPeriodScale(index),		// period scale
		HALSIM_GetPWMZeroLatch(index)			// zero latch
	};
	return data;
#else
	return {};
#endif
}
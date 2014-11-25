/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#pragma once
#ifndef __WPILIB_POWER_DISTRIBUTION_PANEL_H__
#define __WPILIB_POWER_DISTRIBUTION_PANEL_H__

#include "SensorBase.h"

/**
 * Class for getting voltage, current, and temperature from the CAN PDP
 * @author Thomas Clark
 */
class PowerDistributionPanel : public SensorBase {
	public:
		PowerDistributionPanel();
		
		double GetVoltage();
		double GetTemperature();
		double GetCurrent(uint8_t channel);
		double GetTotalCurrent();
		double GetTotalPower();
		double GetTotalEnergy();
		void ResetTotalEnergy();
		void ClearStickyFaults();
};

#endif /* __WPILIB_POWER_DISTRIBUTION_PANEL_H__ */


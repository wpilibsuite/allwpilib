/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "PowerDistributionPanel.h"
#include "WPIErrors.h"
#include "HAL/PDP.hpp"

/**
 * Initialize the PDP.
 */
PowerDistributionPanel::PowerDistributionPanel() {
}

/**
 * @return The voltage of the PDP
 */
double
PowerDistributionPanel::GetVoltage() {
	int32_t status = 0;
	
	double voltage = getPDPVoltage(&status);
	
	if(status) {
		wpi_setWPIErrorWithContext(Timeout, "");
	}
	
	return voltage;
}

/**
 * @return The temperature of the PDP in degrees Celsius
 */
double
PowerDistributionPanel::GetTemperature() {
	int32_t status = 0;
	
	double temperature = getPDPTemperature(&status);
	
	if(status) {
		wpi_setWPIErrorWithContext(Timeout, "");
	}
	
	return temperature;
}

/**
 * @return The current of one of the PDP channels (channels 1-16) in Amperes
 */
double
PowerDistributionPanel::GetCurrent(uint8_t channel) {
	int32_t status = 0;
	
	if(!CheckPDPChannel(channel))
	{
		char buf[64];
		snprintf(buf, 64, "PDP Channel %d", channel);
		wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, buf);
	}
	
	double current = getPDPChannelCurrent(channel, &status);
	
	if(status) {
		wpi_setWPIErrorWithContext(Timeout, "");
	}
	
	return current;
}


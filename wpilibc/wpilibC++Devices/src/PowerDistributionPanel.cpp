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
 * Query the input voltage of the PDP
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
 * Query the temperature of the PDP
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
 * Query the current of a single channel of the PDP
 * @return The current of one of the PDP channels (channels 0-15) in Amperes
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

/**
 * Query the total current of all monitored PDP channels (0-15)
 * @return The the total current drawn from the PDP channels in Amperes
 */
double
PowerDistributionPanel::GetTotalCurrent() {
	int32_t status = 0;
	
	double current = getPDPTotalCurrent(&status);
	
	if(status) {
		wpi_setWPIErrorWithContext(Timeout, "");
	}
	
	return current;
}

/**
 * Query the total power drawn from the monitored PDP channels
 * @return The the total power drawn from the PDP channels in Joules
 */
double
PowerDistributionPanel::GetTotalPower() {
	int32_t status = 0;
	
	double power = getPDPTotalPower(&status);
	
	if(status) {
		wpi_setWPIErrorWithContext(Timeout, "");
	}
	
	return power;
}

/**
 * Query the total energy drawn from the monitored PDP channels
 * @return The the total energy drawn from the PDP channels in Watts
 */
double
PowerDistributionPanel::GetTotalEnergy() {
	int32_t status = 0;
	
	double energy = getPDPTotalEnergy(&status);
	
	if(status) {
		wpi_setWPIErrorWithContext(Timeout, "");
	}
	
	return energy;
}

/**
 * Reset the total energy drawn from the PDP
 * @see PowerDistributionPanel#GetTotalEnergy
 */
void
PowerDistributionPanel::ResetTotalEnergy() { 
	int32_t status = 0;
	
	resetPDPTotalEnergy(&status);
	
	if(status) {
		wpi_setWPIErrorWithContext(Timeout, "");
	}
}

/**
 * Remove all of the fault flags on the PDP
 */
void
PowerDistributionPanel::ClearStickyFaults() {
	int32_t status = 0;
	
	clearPDPStickyFaults(&status);
	
	if(status) {
		wpi_setWPIErrorWithContext(Timeout, "");
	}
}


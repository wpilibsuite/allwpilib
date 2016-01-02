/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/PDP.hpp"
#include "ctre/PDP.h"
//static PDP pdp;

static const int NUM_MODULE_NUMBERS = 63;

static PDP *pdp[NUM_MODULE_NUMBERS] = { NULL };

extern "C" {

void initializePDP(uint8_t module) {
	if(!pdp[module]) {
		pdp[module] = new PDP(module);
	}
}

double getPDPTemperature(uint8_t module, int32_t *status) {
	double temperature;

	*status = pdp[module]->GetTemperature(temperature);

	return temperature;
}

double getPDPVoltage(uint8_t module, int32_t *status) {
	double voltage;

	*status = pdp[module]->GetVoltage(voltage);

	return voltage;
}

double getPDPChannelCurrent(uint8_t module, uint8_t channel, int32_t *status) {
	double current;

	*status = pdp[module]->GetChannelCurrent(channel, current);

	return current;
}

double getPDPTotalCurrent(uint8_t module, int32_t *status) {
	double current;

	*status = pdp[module]->GetTotalCurrent(current);

	return current;
}

double getPDPTotalPower(uint8_t module, int32_t *status) {
	double power;

	*status = pdp[module]->GetTotalPower(power);

	return power;
}

double getPDPTotalEnergy(uint8_t module, int32_t *status) {
	double energy;

	*status = pdp[module]->GetTotalEnergy(energy);

	return energy;
}

void resetPDPTotalEnergy(uint8_t module, int32_t *status) {
	*status = pdp[module]->ResetEnergy();
}

void clearPDPStickyFaults(uint8_t module, int32_t *status) {
	*status = pdp[module]->ClearStickyFaults();
}

}  // extern "C"

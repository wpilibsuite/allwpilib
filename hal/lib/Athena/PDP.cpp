#include "HAL/PDP.hpp"
#include "ctre/PDP.h"
//static PDP pdp;

static const int NUM_MODULE_NUMBERS = 63;

static PDP *pdp[NUM_MODULE_NUMBERS] = { NULL };

void initializePDP(int module) {
	if(!pdp[module]) {
		pdp[module] = new PDP(module);
	}
}

double getPDPTemperature(int32_t *status, uint8_t module) {
	double temperature;

	*status = pdp[module]->GetTemperature(temperature);

	return temperature;
}

double getPDPVoltage(int32_t *status, uint8_t module) {
	double voltage;

	*status = pdp[module]->GetVoltage(voltage);

	return voltage;
}

double getPDPChannelCurrent(uint8_t channel, int32_t *status, uint8_t module) {
	double current;

	*status = pdp[module]->GetChannelCurrent(channel, current);

	return current;
}

double getPDPTotalCurrent(int32_t *status, uint8_t module) {
	double current;

	*status = pdp[module]->GetTotalCurrent(current);

	return current;
}

double getPDPTotalPower(int32_t *status, uint8_t module) {
	double power;

	*status = pdp[module]->GetTotalPower(power);

	return power;
}

double getPDPTotalEnergy(int32_t *status, uint8_t module) {
	double energy;

	*status = pdp[module]->GetTotalEnergy(energy);

	return energy;
}

void resetPDPTotalEnergy(int32_t *status, uint8_t module) {
	*status = pdp[module]->ResetEnergy();
}

void clearPDPStickyFaults(int32_t *status, uint8_t module) {
	*status = pdp[module]->ClearStickyFaults();
}



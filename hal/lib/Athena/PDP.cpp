#include "HAL/PDP.hpp"
#include "ctre/PDP.h"
static PDP pdp;

double getPDPTemperature(int32_t *status) {
	double temperature;
	
	*status = pdp.GetTemperature(temperature);
	
	return temperature;
}

double getPDPVoltage(int32_t *status) {
	double voltage;
	
	*status = pdp.GetVoltage(voltage);
	
	return voltage;
}

double getPDPChannelCurrent(uint8_t channel, int32_t *status) {
	double current;
	
	*status = pdp.GetChannelCurrent(channel, current);
	
	return current;
}

double getPDPTotalCurrent(int32_t *status) {
	double current;
	
	*status = pdp.GetTotalCurrent(current);
	
	return current;
}

double getPDPTotalPower(int32_t *status) {
	double power;
	
	*status = pdp.GetTotalPower(power);
	
	return power;
}

double getPDPTotalEnergy(int32_t *status) {
	double energy;
	
	*status = pdp.GetTotalEnergy(energy);
	
	return energy;
}

void resetPDPTotalEnergy(int32_t *status) {
	*status = pdp.ResetEnergy();
}

void clearPDPStickyFaults(int32_t *status) {
	*status = pdp.ClearStickyFaults();
}



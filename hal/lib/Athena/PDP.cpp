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


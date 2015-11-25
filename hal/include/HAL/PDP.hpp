#pragma once

#include <stdint.h>

extern "C"
{
	void initializePDP(uint8_t module);
	double getPDPTemperature(uint8_t module, int32_t *status);
	double getPDPVoltage(uint8_t module, int32_t *status);
	double getPDPChannelCurrent(uint8_t module, uint8_t channel, int32_t *status);
	double getPDPTotalCurrent(uint8_t module, int32_t *status);
	double getPDPTotalPower(uint8_t module, int32_t *status);
	double getPDPTotalEnergy(uint8_t module, int32_t *status);
	void resetPDPTotalEnergy(uint8_t module, int32_t *status);
	void clearPDPStickyFaults(uint8_t module, int32_t *status);
}

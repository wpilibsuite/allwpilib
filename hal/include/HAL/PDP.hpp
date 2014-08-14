#pragma once

#include <stdint.h>

extern "C"
{
	void initializePDP(int module);
	double getPDPTemperature(int32_t *status, uint8_t module);
	double getPDPVoltage(int32_t *status, uint8_t module);
	double getPDPChannelCurrent(uint8_t channel, int32_t *status, uint8_t module);
	double getPDPTotalCurrent(int32_t *status, uint8_t module);
	double getPDPTotalPower(int32_t *status, uint8_t module);
	double getPDPTotalEnergy(int32_t *status, uint8_t module);
	void resetPDPTotalEnergy(int32_t *status, uint8_t module);
	void clearPDPStickyFaults(int32_t *status, uint8_t module);
}

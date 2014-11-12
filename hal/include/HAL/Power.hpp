#pragma once

#include <stdint.h>

extern "C"
{
	float getVinVoltage(int32_t *status);
	float getVinCurrent(int32_t *status);
	float getUserVoltage6V(int32_t *status);
	float getUserCurrent6V(int32_t *status);
	bool getUserActive6V(int32_t *status);
	int getUserCurrentFaults6V(int32_t *status);
	float getUserVoltage5V(int32_t *status);
	float getUserCurrent5V(int32_t *status);
	bool getUserActive5V(int32_t *status);
	int getUserCurrentFaults5V(int32_t *status);
	float getUserVoltage3V3(int32_t *status);
	float getUserCurrent3V3(int32_t *status);
	bool getUserActive3V3(int32_t *status);
	int getUserCurrentFaults3V3(int32_t *status);
}

#include "HAL/Power.hpp"
#include "ChipObject.h"

static tPower *power = NULL;

static void initializePower(int32_t *status) {
	if(power == NULL) {
		power = tPower::create(status);
	}
}

/**
 * Get the roboRIO input voltage
 */
float getVinVoltage(int32_t *status) {
	initializePower(status);
	return power->readVinVoltage(status) / 4.096f * 0.025733f - 0.029f;
}

/**
 * Get the roboRIO input current
 */
float getVinCurrent(int32_t *status) {
	initializePower(status);
	return power->readVinCurrent(status) / 4.096f * 0.017042 - 0.071f;
}

/**
 * Get the 6V rail voltage
 */
float getUserVoltage6V(int32_t *status) {
	initializePower(status);
	return power->readUserVoltage6V(status) / 4.096f * 0.007019f - 0.014f;
}

/**
 * Get the 6V rail current
 */
float getUserCurrent6V(int32_t *status) {
	initializePower(status);
	return power->readUserCurrent6V(status) / 4.096f * 0.005566f - 0.009f;
}

/**
 * Get the active state of the 6V rail
 */
bool getUserActive6V(int32_t *status) {
	initializePower(status);
	return power->readStatus_User6V(status) == 4;
}

/**
 * Get the fault count for the 6V rail
 */
int getUserCurrentFaults6V(int32_t *status) {
	initializePower(status);
	return (int)power->readOverCurrentFaultCounts_OverCurrentFaultCount6V(status);
}

/**
 * Get the 5V rail voltage
 */
float getUserVoltage5V(int32_t *status) {
	initializePower(status);
	return power->readUserVoltage5V(status) / 4.096f * 0.005962f - 0.013f;
}

/**
 * Get the 5V rail current
 */
float getUserCurrent5V(int32_t *status) {
	initializePower(status);
	return power->readUserCurrent5V(status) / 4.096f * 0.001996f - 0.002f;
}

/**
 * Get the active state of the 5V rail
 */
bool getUserActive5V(int32_t *status) {
	initializePower(status);
	return power->readStatus_User5V(status) == 4;
}

/**
 * Get the fault count for the 5V rail
 */
int getUserCurrentFaults5V(int32_t *status) {
	initializePower(status);
	return (int)power->readOverCurrentFaultCounts_OverCurrentFaultCount5V(status);
}

unsigned char getUserStatus5V(int32_t *status) {
	initializePower(status);
	return power->readStatus_User5V(status);
}

/**
 * Get the 3.3V rail voltage
 */
float getUserVoltage3V3(int32_t *status) {
	initializePower(status);
	return power->readUserVoltage3V3(status) / 4.096f * 0.004902f - 0.01f;
}

/**
 * Get the 3.3V rail current
 */
float getUserCurrent3V3(int32_t *status) {
	initializePower(status);
	return power->readUserCurrent3V3(status) / 4.096f * 0.002486f - 0.003f;
}

/**
 * Get the active state of the 3.3V rail
 */
bool getUserActive3V3(int32_t *status) {
	initializePower(status);
	return power->readStatus_User3V3(status) == 4;
}

/**
 * Get the fault count for the 3.3V rail
 */
int getUserCurrentFaults3V3(int32_t *status) {
	initializePower(status);
	return (int)power->readOverCurrentFaultCounts_OverCurrentFaultCount3V3(status);
}

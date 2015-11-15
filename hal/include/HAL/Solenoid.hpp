#pragma once

#include <stdint.h>

extern "C"
{
	void* initializeSolenoidPort(void* port_pointer, int32_t *status);
	void freeSolenoidPort(void* solenoid_port_pointer);
	bool checkSolenoidModule(uint8_t module);

	bool getSolenoid(void* solenoid_port_pointer, int32_t *status);
	uint8_t getAllSolenoids(void* solenoid_port_pointer, int32_t *status);
	void setSolenoid(void* solenoid_port_pointer, bool value, int32_t *status);

	int getPCMSolenoidBlackList(void* solenoid_port_pointer, int32_t *status);
	bool getPCMSolenoidVoltageStickyFault(void* solenoid_port_pointer, int32_t *status);
	bool getPCMSolenoidVoltageFault(void* solenoid_port_pointer, int32_t *status);
	void clearAllPCMStickyFaults_sol(void *solenoid_port_pointer, int32_t *status);
}

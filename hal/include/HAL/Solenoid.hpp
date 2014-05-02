#pragma once

#ifdef __vxworks
#include <vxWorks.h>
#else
#include <stdint.h>
#endif

extern "C"
{
	void* initializeSolenoidPort(void* port_pointer, int32_t *status);
	bool checkSolenoidModule(uint8_t module);

	bool getSolenoid(void* solenoid_port_pointer, int32_t *status);
	void setSolenoid(void* solenoid_port_pointer, bool value, int32_t *status);
}

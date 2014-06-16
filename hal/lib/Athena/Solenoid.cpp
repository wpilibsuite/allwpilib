
#include "HAL/Solenoid.hpp"

#include "Port.h"
#include "HAL/Errors.hpp"
#include "ChipObject.h"
#include "HAL/cpp/Synchronized.hpp"
#include "NetworkCommunication/LoadOut.h"
#include "ctre/PCM.h"

bool pcmModulesInitialized = false;

static const int NUM_PCMS = 2;
PCM *modules[NUM_PCMS];

struct solenoid_port_t {
	PCM *module;
	uint32_t pin;
};

void initializePCM() {
	if(!pcmModulesInitialized) {
		modules[0] = new PCM(50);
		modules[1] = new PCM(51);

		pcmModulesInitialized = true;
	}
}

void* initializeSolenoidPort(void *port_pointer, int32_t *status) {
	initializePCM();

	Port* port = (Port*) port_pointer;

	solenoid_port_t *solenoid_port = new solenoid_port_t;
	solenoid_port->module = modules[port->module - 1];
	solenoid_port->pin = port->pin;

	return solenoid_port;
}

bool checkSolenoidModule(uint8_t module) {
	return module > 0 and module <= NUM_PCMS;
}

bool getSolenoid(void* solenoid_port_pointer, int32_t *status) {
	solenoid_port_t* port = (solenoid_port_t*) solenoid_port_pointer;
	bool value;

	*status = port->module->GetSolenoid(port->pin, value);

	return value;
}

void setSolenoid(void* solenoid_port_pointer, bool value, int32_t *status) {
	solenoid_port_t* port = (solenoid_port_t*) solenoid_port_pointer;

	port->module->SetSolenoid(port->pin, value);
}

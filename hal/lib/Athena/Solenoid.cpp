
#include "HAL/Solenoid.hpp"

#include "Port.h"
#include "HAL/Errors.hpp"
#include "ChipObject.h"
#include "HAL/cpp/Synchronized.hpp"
#include "NetworkCommunication/LoadOut.h"
#include "ctre/PCM.h"

static const int NUM_MODULE_NUMBERS = 63;

PCM *modules[NUM_MODULE_NUMBERS] = { NULL };

struct solenoid_port_t {
	PCM *module;
	uint32_t pin;
};

void initializePCM(int module) {
	if(!modules[module]) {
		modules[module] = new PCM(module);
	}
}

void* initializeSolenoidPort(void *port_pointer, int32_t *status) {
	Port* port = (Port*) port_pointer;
	initializePCM(port->module);
	
	solenoid_port_t *solenoid_port = new solenoid_port_t;
	solenoid_port->module = modules[port->module];
	solenoid_port->pin = port->pin;

	return solenoid_port;
}

bool checkSolenoidModule(uint8_t module) {
	return module < NUM_MODULE_NUMBERS;
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

int getPCMSolenoidBlackList(void* solenoid_port_pointer, int32_t *status){
	solenoid_port_t* port = (solenoid_port_t*) solenoid_port_pointer;
	UINT8 value;
	
	*status = port->module->GetSolenoidBlackList(value);

	return value;
}
bool getPCMSolenoidVoltageStickyFault(void* solenoid_port_pointer, int32_t *status){
	solenoid_port_t* port = (solenoid_port_t*) solenoid_port_pointer;
	bool value;

	*status = port->module->GetSolenoidStickyFault(value);

	return value;
}
bool getPCMSolenoidVoltageFault(void* solenoid_port_pointer, int32_t *status){
	solenoid_port_t* port = (solenoid_port_t*) solenoid_port_pointer;
	bool value;

	*status = port->module->GetSolenoidFault(value);

	return value;
}
void clearAllPCMStickyFaults_sol(void *solenoid_port_pointer, int32_t *status){
	solenoid_port_t* port = (solenoid_port_t*) solenoid_port_pointer;
	
	*status = port->module->ClearStickyFaults();
}

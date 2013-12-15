
#include "HAL/Solenoid.h"

#include "Port.h"
#include "HAL/Errors.h"
#include "ChipObject.h"
#include "HAL/cpp/Synchronized.h"
#include "NetworkCommunication/LoadOut.h"

// XXX No solenoid abstraction :(

// struct solenoid_port_t {
//   Port port;
//   tSolenoid *module;
//   uint32_t PWMGeneratorID;
// };
// typedef struct solenoid_port_t SolenoidPort;

// static ReentrantSemaphore solenoidSemaphore;
// static tSolenoid* solenoidModules[2] = {NULL, NULL};

// bool solenoidModulesInitialized = false;

// /**
//  * Initialize the digital modules.
//  */
// void initializeSolenoid(int32_t *status) {
//   if (solenoidModulesInitialized) return;

//   for (unsigned int i = 0; i < (sizeof(solenoidModules)/sizeof(solenoidModules[0])); i++) {
// 	Synchronized sync(solenoidSemaphore);
// 	solenoidModules[i] = tSolenoid::create(status);
//   }
//   solenoidModulesInitialized = true;
// }

// void* initializeSolenoidPort(void* port_pointer, int32_t *status) {
//   initializeSolenoid(status);
//   Port* port = (Port*) port_pointer;

//   // Initialize port structure
//   SolenoidPort* solenoid_port = new SolenoidPort();
//   solenoid_port->port = *port;
//   solenoid_port->module = solenoidModules[solenoid_port->port.module-1];

//   return solenoid_port;
// }

// bool checkSolenoidModule(uint8_t module) {
//   if (nLoadOut::getModulePresence(nLoadOut::kModuleType_Solenoid, module - 1))
// 	return true;
//   return false;
// }

// bool getSolenoid(void* solenoid_port_pointer, int32_t *status) {
//   SolenoidPort* port = (SolenoidPort*) solenoid_port_pointer;
//   if (checkSolenoidModule(port->port.module)) {
// 	uint8_t mask = 1 << (port->port.pin - 1);
// 	return (mask & port->module->readDO7_0(port->port.module - 1, status));
//   }
//   return false;
// }

// void setSolenoid(void* solenoid_port_pointer, bool value, int32_t *status) {
//   SolenoidPort* port = (SolenoidPort*) solenoid_port_pointer;
//   if (checkSolenoidModule(port->port.module)) {
// 	Synchronized sync(solenoidSemaphore);
// 	uint8_t currentValue = port->module->readDO7_0(port->port.module - 1, status);
// 	uint8_t mask = 1 << (port->port.pin - 1);
// 	if (value) currentValue = currentValue | mask; // Flip the bit on
// 	else currentValue = currentValue & ~mask; // Flip the bit off
// 	port->module->writeDO7_0(port->port.module - 1, currentValue, status);
//   }
// }

// XXX No solenoid abstraction :(

/**
 * Initialize the digital modules.
 */
void initializeSolenoid(int32_t *status) {}
void* initializeSolenoidPort(void* port_pointer, int32_t *status) { return NULL; }
bool checkSolenoidModule(uint8_t module) { return false; }
bool getSolenoid(void* solenoid_port_pointer, int32_t *status) { return false; }
void setSolenoid(void* solenoid_port_pointer, bool value, int32_t *status) {}


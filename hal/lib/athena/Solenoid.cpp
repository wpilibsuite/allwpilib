/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Solenoid.h"

#include "ChipObject.h"
#include "FRC_NetworkCommunication/LoadOut.h"
#include "HAL/Errors.h"
#include "ctre/PCM.h"
#include "handles/HandlesInternal.h"

static const int NUM_MODULE_NUMBERS = 63;

PCM* PCM_modules[NUM_MODULE_NUMBERS] = {nullptr};

struct solenoid_port_t {
  PCM* module;
  uint32_t pin;
};

void initializePCM(int module) {
  if (!PCM_modules[module]) {
    PCM_modules[module] = new PCM(module);
  }
}

using namespace hal;

extern "C" {

void* initializeSolenoidPort(HalPortHandle port_handle, int32_t* status) {
  int16_t pin = getPortHandlePin(port_handle);
  int16_t module = getPortHandleModule(port_handle);
  if (pin == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return nullptr;
  }

  initializePCM(module);

  solenoid_port_t* solenoid_port = new solenoid_port_t;
  solenoid_port->module = PCM_modules[module];
  solenoid_port->pin = pin;

  return solenoid_port;
}

void freeSolenoidPort(void* solenoid_port_pointer) {
  solenoid_port_t* port = (solenoid_port_t*)solenoid_port_pointer;
  delete port;
}

bool checkSolenoidModule(uint8_t module) { return module < NUM_MODULE_NUMBERS; }

bool getSolenoid(void* solenoid_port_pointer, int32_t* status) {
  solenoid_port_t* port = (solenoid_port_t*)solenoid_port_pointer;
  bool value;

  *status = port->module->GetSolenoid(port->pin, value);

  return value;
}

uint8_t getAllSolenoids(void* solenoid_port_pointer, int32_t* status) {
  solenoid_port_t* port = (solenoid_port_t*)solenoid_port_pointer;
  uint8_t value;

  *status = port->module->GetAllSolenoids(value);

  return value;
}

void setSolenoid(void* solenoid_port_pointer, bool value, int32_t* status) {
  solenoid_port_t* port = (solenoid_port_t*)solenoid_port_pointer;

  *status = port->module->SetSolenoid(port->pin, value);
}

int getPCMSolenoidBlackList(void* solenoid_port_pointer, int32_t* status) {
  solenoid_port_t* port = (solenoid_port_t*)solenoid_port_pointer;
  UINT8 value;

  *status = port->module->GetSolenoidBlackList(value);

  return value;
}
bool getPCMSolenoidVoltageStickyFault(void* solenoid_port_pointer,
                                      int32_t* status) {
  solenoid_port_t* port = (solenoid_port_t*)solenoid_port_pointer;
  bool value;

  *status = port->module->GetSolenoidStickyFault(value);

  return value;
}
bool getPCMSolenoidVoltageFault(void* solenoid_port_pointer, int32_t* status) {
  solenoid_port_t* port = (solenoid_port_t*)solenoid_port_pointer;
  bool value;

  *status = port->module->GetSolenoidFault(value);

  return value;
}
void clearAllPCMStickyFaults_sol(void* solenoid_port_pointer, int32_t* status) {
  solenoid_port_t* port = (solenoid_port_t*)solenoid_port_pointer;

  *status = port->module->ClearStickyFaults();
}

}  // extern "C"

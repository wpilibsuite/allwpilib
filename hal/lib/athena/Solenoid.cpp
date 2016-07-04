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
#include "HAL/Ports.h"
#include "PCMInternal.h"
#include "PortsInternal.h"
#include "ctre/PCM.h"
#include "handles/HandlesInternal.h"
#include "handles/IndexedHandleResource.h"

namespace {
struct Solenoid {
  uint8_t module;
  uint8_t pin;
};
}

using namespace hal;

static IndexedHandleResource<HalSolenoidHandle, Solenoid,
                             kNumPCMModules * kNumSolenoidPins,
                             HalHandleEnum::Solenoid>
    solenoidHandles;

extern "C" {

HalSolenoidHandle initializeSolenoidPort(HalPortHandle port_handle,
                                         int32_t* status) {
  int16_t pin = getPortHandlePin(port_handle);
  int16_t module = getPortHandleModule(port_handle);
  if (pin == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;  // Change to Handle Error
    return HAL_INVALID_HANDLE;
  }

  if (module >= kNumPCMModules || pin >= kNumSolenoidPins) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_INVALID_HANDLE;
  }

  initializePCM(module);

  auto handle =
      solenoidHandles.Allocate(module * kNumSolenoidPins + pin, status);
  if (handle == HAL_INVALID_HANDLE) {  // out of resources
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_INVALID_HANDLE;
  }
  auto solenoid_port = solenoidHandles.Get(handle);
  if (solenoid_port == nullptr) {  // would only occur on thread issues
    *status = HAL_HANDLE_ERROR;
    return HAL_INVALID_HANDLE;
  }
  solenoid_port->module = static_cast<uint8_t>(module);
  solenoid_port->pin = static_cast<uint8_t>(pin);

  return handle;
}

void freeSolenoidPort(HalSolenoidHandle solenoid_port_handle) {
  solenoidHandles.Free(solenoid_port_handle);
}

bool checkSolenoidModule(uint8_t module) { return module < kNumPCMModules; }

bool getSolenoid(HalSolenoidHandle solenoid_port_handle, int32_t* status) {
  auto port = solenoidHandles.Get(solenoid_port_handle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  bool value;

  *status = PCM_modules[port->module]->GetSolenoid(port->pin, value);

  return value;
}

uint8_t getAllSolenoids(uint8_t module, int32_t* status) {
  if (module >= kNumPCMModules) {
    *status = PARAMETER_OUT_OF_RANGE;
    return 0;
  }
  uint8_t value;

  *status = PCM_modules[module]->GetAllSolenoids(value);

  return value;
}

void setSolenoid(HalSolenoidHandle solenoid_port_handle, bool value,
                 int32_t* status) {
  auto port = solenoidHandles.Get(solenoid_port_handle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  *status = PCM_modules[port->module]->SetSolenoid(port->pin, value);
}

int getPCMSolenoidBlackList(uint8_t module, int32_t* status) {
  if (module >= kNumPCMModules) {
    *status = PARAMETER_OUT_OF_RANGE;
    return 0;
  }
  UINT8 value;

  *status = PCM_modules[module]->GetSolenoidBlackList(value);

  return value;
}
bool getPCMSolenoidVoltageStickyFault(uint8_t module, int32_t* status) {
  if (module >= kNumPCMModules) {
    *status = PARAMETER_OUT_OF_RANGE;
    return false;
  }
  bool value;

  *status = PCM_modules[module]->GetSolenoidStickyFault(value);

  return value;
}
bool getPCMSolenoidVoltageFault(uint8_t module, int32_t* status) {
  if (module >= kNumPCMModules) {
    *status = PARAMETER_OUT_OF_RANGE;
    return false;
  }
  bool value;

  *status = PCM_modules[module]->GetSolenoidFault(value);

  return value;
}
void clearAllPCMStickyFaults_sol(uint8_t module, int32_t* status) {
  if (module >= kNumPCMModules) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  *status = PCM_modules[module]->ClearStickyFaults();
}

}  // extern "C"

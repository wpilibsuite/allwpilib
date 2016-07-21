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
#include "HAL/handles/HandlesInternal.h"
#include "HAL/handles/IndexedHandleResource.h"
#include "PCMInternal.h"
#include "PortsInternal.h"
#include "ctre/PCM.h"

namespace {
struct Solenoid {
  uint8_t module;
  uint8_t pin;
};
}

using namespace hal;

static IndexedHandleResource<HAL_SolenoidHandle, Solenoid,
                             kNumPCMModules * kNumSolenoidPins,
                             HAL_HandleEnum::Solenoid>
    solenoidHandles;

extern "C" {

HAL_SolenoidHandle HAL_InitializeSolenoidPort(HAL_PortHandle port_handle,
                                              int32_t* status) {
  int16_t pin = getPortHandlePin(port_handle);
  int16_t module = getPortHandleModule(port_handle);
  if (pin == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  // initializePCM will check the module
  if (!HAL_CheckSolenoidChannel(pin)) {
    *status = RESOURCE_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }

  initializePCM(module, status);
  if (*status != 0) {
    return HAL_kInvalidHandle;
  }

  auto handle =
      solenoidHandles.Allocate(module * kNumSolenoidPins + pin, status);
  if (handle == HAL_kInvalidHandle) {  // out of resources
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }
  auto solenoid_port = solenoidHandles.Get(handle);
  if (solenoid_port == nullptr) {  // would only occur on thread issues
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  solenoid_port->module = static_cast<uint8_t>(module);
  solenoid_port->pin = static_cast<uint8_t>(pin);

  return handle;
}

void HAL_FreeSolenoidPort(HAL_SolenoidHandle solenoid_port_handle) {
  solenoidHandles.Free(solenoid_port_handle);
}

HAL_Bool HAL_CheckSolenoidModule(int32_t module) {
  return module < kNumPCMModules && module >= 0;
}

HAL_Bool HAL_CheckSolenoidChannel(int32_t pin) {
  return pin < kNumSolenoidPins && pin >= 0;
}

HAL_Bool HAL_GetSolenoid(HAL_SolenoidHandle solenoid_port_handle,
                         int32_t* status) {
  auto port = solenoidHandles.Get(solenoid_port_handle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  bool value;

  *status = PCM_modules[port->module]->GetSolenoid(port->pin, value);

  return value;
}

int32_t HAL_GetAllSolenoids(int32_t module, int32_t* status) {
  if (!checkPCMInit(module, status)) return 0;
  uint8_t value;

  *status = PCM_modules[module]->GetAllSolenoids(value);

  return value;
}

void HAL_SetSolenoid(HAL_SolenoidHandle solenoid_port_handle, HAL_Bool value,
                     int32_t* status) {
  auto port = solenoidHandles.Get(solenoid_port_handle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  *status = PCM_modules[port->module]->SetSolenoid(port->pin, value);
}

int32_t HAL_GetPCMSolenoidBlackList(int32_t module, int32_t* status) {
  if (!checkPCMInit(module, status)) return 0;
  UINT8 value;

  *status = PCM_modules[module]->GetSolenoidBlackList(value);

  return value;
}
HAL_Bool HAL_GetPCMSolenoidVoltageStickyFault(int32_t module, int32_t* status) {
  if (!checkPCMInit(module, status)) return 0;
  bool value;

  *status = PCM_modules[module]->GetSolenoidStickyFault(value);

  return value;
}
HAL_Bool HAL_GetPCMSolenoidVoltageFault(int32_t module, int32_t* status) {
  if (!checkPCMInit(module, status)) return false;
  bool value;

  *status = PCM_modules[module]->GetSolenoidFault(value);

  return value;
}
void HAL_ClearAllPCMStickyFaults(int32_t module, int32_t* status) {
  if (!checkPCMInit(module, status)) return;

  *status = PCM_modules[module]->ClearStickyFaults();
}

}  // extern "C"

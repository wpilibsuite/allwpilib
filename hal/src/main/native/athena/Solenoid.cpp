/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Solenoid.h"

#include <FRC_NetworkCommunication/LoadOut.h>

#include "HALInitializer.h"
#include "PCMInternal.h"
#include "PortsInternal.h"
#include "ctre/PCM.h"
#include "hal/ChipObject.h"
#include "hal/Errors.h"
#include "hal/Ports.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/IndexedHandleResource.h"

namespace {

struct Solenoid {
  uint8_t module;
  uint8_t channel;
};

}  // namespace

using namespace hal;

static IndexedHandleResource<HAL_SolenoidHandle, Solenoid,
                             kNumPCMModules * kNumSolenoidChannels,
                             HAL_HandleEnum::Solenoid>* solenoidHandles;

namespace hal {
namespace init {
void InitializeSolenoid() {
  static IndexedHandleResource<HAL_SolenoidHandle, Solenoid,
                               kNumPCMModules * kNumSolenoidChannels,
                               HAL_HandleEnum::Solenoid>
      sH;
  solenoidHandles = &sH;
}
}  // namespace init
}  // namespace hal

extern "C" {

HAL_SolenoidHandle HAL_InitializeSolenoidPort(HAL_PortHandle portHandle,
                                              int32_t* status) {
  hal::init::CheckInit();
  int16_t channel = getPortHandleChannel(portHandle);
  int16_t module = getPortHandleModule(portHandle);
  if (channel == InvalidHandleIndex) {
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  // initializePCM will check the module
  if (!HAL_CheckSolenoidChannel(channel)) {
    *status = RESOURCE_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }

  initializePCM(module, status);
  if (*status != 0) {
    return HAL_kInvalidHandle;
  }

  auto handle = solenoidHandles->Allocate(
      module * kNumSolenoidChannels + channel, status);
  if (*status != 0) {
    return HAL_kInvalidHandle;
  }
  auto solenoidPort = solenoidHandles->Get(handle);
  if (solenoidPort == nullptr) {  // would only occur on thread issues
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  solenoidPort->module = static_cast<uint8_t>(module);
  solenoidPort->channel = static_cast<uint8_t>(channel);

  return handle;
}

void HAL_FreeSolenoidPort(HAL_SolenoidHandle solenoidPortHandle) {
  solenoidHandles->Free(solenoidPortHandle);
}

HAL_Bool HAL_CheckSolenoidModule(int32_t module) {
  return module < kNumPCMModules && module >= 0;
}

HAL_Bool HAL_CheckSolenoidChannel(int32_t channel) {
  return channel < kNumSolenoidChannels && channel >= 0;
}

HAL_Bool HAL_GetSolenoid(HAL_SolenoidHandle solenoidPortHandle,
                         int32_t* status) {
  auto port = solenoidHandles->Get(solenoidPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  bool value;

  *status = PCM_modules[port->module]->GetSolenoid(port->channel, value);

  return value;
}

int32_t HAL_GetAllSolenoids(int32_t module, int32_t* status) {
  if (!checkPCMInit(module, status)) return 0;
  uint8_t value;

  *status = PCM_modules[module]->GetAllSolenoids(value);

  return value;
}

void HAL_SetSolenoid(HAL_SolenoidHandle solenoidPortHandle, HAL_Bool value,
                     int32_t* status) {
  auto port = solenoidHandles->Get(solenoidPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  *status = PCM_modules[port->module]->SetSolenoid(port->channel, value);
}

void HAL_SetAllSolenoids(int32_t module, int32_t state, int32_t* status) {
  if (!checkPCMInit(module, status)) return;

  *status = PCM_modules[module]->SetAllSolenoids(state);
}

int32_t HAL_GetPCMSolenoidBlackList(int32_t module, int32_t* status) {
  if (!checkPCMInit(module, status)) return 0;
  uint8_t value;

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

void HAL_SetOneShotDuration(HAL_SolenoidHandle solenoidPortHandle,
                            int32_t durMS, int32_t* status) {
  auto port = solenoidHandles->Get(solenoidPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  *status =
      PCM_modules[port->module]->SetOneShotDurationMs(port->channel, durMS);
}

void HAL_FireOneShot(HAL_SolenoidHandle solenoidPortHandle, int32_t* status) {
  auto port = solenoidHandles->Get(solenoidPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  *status = PCM_modules[port->module]->FireOneShotSolenoid(port->channel);
}
}  // extern "C"

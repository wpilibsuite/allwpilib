/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Solenoid.h"

#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/Errors.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/IndexedHandleResource.h"
#include "mockdata/PCMDataInternal.h"

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

  if (!HAL_CheckSolenoidChannel(channel)) {
    *status = RESOURCE_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }

  if (!HAL_CheckSolenoidModule(module)) {
    *status = RESOURCE_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }

  auto handle = solenoidHandles->Allocate(
      module * kNumSolenoidChannels + channel, status);
  if (handle == HAL_kInvalidHandle) {  // out of resources
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }
  auto solenoidPort = solenoidHandles->Get(handle);
  if (solenoidPort == nullptr) {  // would only occur on thread issues
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  solenoidPort->module = static_cast<uint8_t>(module);
  solenoidPort->channel = static_cast<uint8_t>(channel);

  HALSIM_SetPCMSolenoidInitialized(module, channel, true);

  return handle;
}
void HAL_FreeSolenoidPort(HAL_SolenoidHandle solenoidPortHandle) {
  auto port = solenoidHandles->Get(solenoidPortHandle);
  if (port == nullptr) return;
  solenoidHandles->Free(solenoidPortHandle);
  HALSIM_SetPCMSolenoidInitialized(port->module, port->channel, false);
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

  return HALSIM_GetPCMSolenoidOutput(port->module, port->channel);
}
int32_t HAL_GetAllSolenoids(int32_t module, int32_t* status) {
  int32_t total = 0;
  for (int i = 0; i < kNumSolenoidChannels; i++) {
    int32_t channel = HALSIM_GetPCMSolenoidOutput(module, i) ? 1 : 0;
    total = total + (channel << i);
  }

  return total;
}
void HAL_SetSolenoid(HAL_SolenoidHandle solenoidPortHandle, HAL_Bool value,
                     int32_t* status) {
  auto port = solenoidHandles->Get(solenoidPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  HALSIM_SetPCMSolenoidOutput(port->module, port->channel, value);
}
int32_t HAL_GetPCMSolenoidBlackList(int32_t module, int32_t* status) {
  return 0;
}
HAL_Bool HAL_GetPCMSolenoidVoltageStickyFault(int32_t module, int32_t* status) {
  return 0;
}
HAL_Bool HAL_GetPCMSolenoidVoltageFault(int32_t module, int32_t* status) {
  return 0;
}
void HAL_ClearAllPCMStickyFaults(int32_t module, int32_t* status) {}
void HAL_SetOneShotDuration(HAL_SolenoidHandle solenoidPortHandle,
                            int32_t durMS, int32_t* status) {}
void HAL_FireOneShot(HAL_SolenoidHandle solenoidPortHandle, int32_t* status) {}
}  // extern "C"

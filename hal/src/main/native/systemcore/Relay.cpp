// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Relay.h"

#include <string>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/handles/IndexedHandleResource.h"

using namespace hal;

namespace hal::init {
void InitializeRelay() {}
}  // namespace hal::init

extern "C" {

HAL_RelayHandle HAL_InitializeRelayPort(HAL_PortHandle portHandle, HAL_Bool fwd,
                                        const char* allocationLocation,
                                        int32_t* status) {
  hal::init::CheckInit();
  *status = HAL_HANDLE_ERROR;
  return HAL_kInvalidHandle;
}

void HAL_FreeRelayPort(HAL_RelayHandle relayPortHandle) {}

HAL_Bool HAL_CheckRelayChannel(int32_t channel) {
  return false;
}

void HAL_SetRelay(HAL_RelayHandle relayPortHandle, HAL_Bool on,
                  int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

HAL_Bool HAL_GetRelay(HAL_RelayHandle relayPortHandle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return false;
}

}  // extern "C"

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Relay.h"

#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/handles/IndexedHandleResource.h"
#include "mockdata/RelayDataInternal.h"

using namespace hal;

namespace {
struct Relay {
  uint8_t channel;
  bool fwd;
};
}  // namespace

static IndexedHandleResource<HAL_RelayHandle, Relay, kNumRelayChannels,
                             HAL_HandleEnum::Relay>* relayHandles;

namespace hal {
namespace init {
void InitializeRelay() {
  static IndexedHandleResource<HAL_RelayHandle, Relay, kNumRelayChannels,
                               HAL_HandleEnum::Relay>
      rH;
  relayHandles = &rH;
}
}  // namespace init
}  // namespace hal

extern "C" {
HAL_RelayHandle HAL_InitializeRelayPort(HAL_PortHandle portHandle, HAL_Bool fwd,
                                        int32_t* status) {
  hal::init::CheckInit();
  if (*status != 0) return HAL_kInvalidHandle;

  int16_t channel = getPortHandleChannel(portHandle);
  if (channel == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }

  if (!fwd) channel += kNumRelayHeaders;  // add 4 to reverse channels

  auto handle = relayHandles->Allocate(channel, status);

  if (*status != 0)
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.

  auto port = relayHandles->Get(handle);
  if (port == nullptr) {  // would only occur on thread issue.
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  if (!fwd) {
    // Subtract number of headers to put channel in range
    channel -= kNumRelayHeaders;

    port->fwd = false;  // set to reverse

    SimRelayData[channel].initializedReverse = true;
  } else {
    port->fwd = true;  // set to forward
    SimRelayData[channel].initializedForward = true;
  }

  port->channel = static_cast<uint8_t>(channel);

  return handle;
}

void HAL_FreeRelayPort(HAL_RelayHandle relayPortHandle) {
  auto port = relayHandles->Get(relayPortHandle);
  relayHandles->Free(relayPortHandle);
  if (port == nullptr) return;
  if (port->fwd)
    SimRelayData[port->channel].initializedForward = false;
  else
    SimRelayData[port->channel].initializedReverse = false;
}

HAL_Bool HAL_CheckRelayChannel(int32_t channel) {
  // roboRIO only has 4 headers, and the FPGA has
  // seperate functions for forward and reverse,
  // instead of seperate channel IDs
  return channel < kNumRelayHeaders && channel >= 0;
}

void HAL_SetRelay(HAL_RelayHandle relayPortHandle, HAL_Bool on,
                  int32_t* status) {
  auto port = relayHandles->Get(relayPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (port->fwd)
    SimRelayData[port->channel].forward = on;
  else
    SimRelayData[port->channel].reverse = on;
}

HAL_Bool HAL_GetRelay(HAL_RelayHandle relayPortHandle, int32_t* status) {
  auto port = relayHandles->Get(relayPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  if (port->fwd)
    return SimRelayData[port->channel].forward;
  else
    return SimRelayData[port->channel].reverse;
}
}  // extern "C"

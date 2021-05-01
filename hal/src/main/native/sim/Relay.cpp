// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Relay.h"

#include <string>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/handles/IndexedHandleResource.h"
#include "mockdata/RelayDataInternal.h"

using namespace hal;

namespace {
struct Relay {
  uint8_t channel;
  bool fwd;
  std::string previousAllocation;
};
}  // namespace

static IndexedHandleResource<HAL_RelayHandle, Relay, kNumRelayChannels,
                             HAL_HandleEnum::Relay>* relayHandles;

namespace hal::init {
void InitializeRelay() {
  static IndexedHandleResource<HAL_RelayHandle, Relay, kNumRelayChannels,
                               HAL_HandleEnum::Relay>
      rH;
  relayHandles = &rH;
}
}  // namespace hal::init

extern "C" {
HAL_RelayHandle HAL_InitializeRelayPort(HAL_PortHandle portHandle, HAL_Bool fwd,
                                        const char* allocationLocation,
                                        int32_t* status) {
  hal::init::CheckInit();
  if (*status != 0) {
    return HAL_kInvalidHandle;
  }

  int16_t channel = getPortHandleChannel(portHandle);
  if (channel == InvalidHandleIndex || channel >= kNumRelayChannels) {
    *status = RESOURCE_OUT_OF_RANGE;
    hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for Relay", 0,
                                     kNumRelayChannels, channel);
    return HAL_kInvalidHandle;
  }

  if (!fwd) {
    channel += kNumRelayHeaders;  // add 4 to reverse channels
  }

  HAL_RelayHandle handle;
  auto port = relayHandles->Allocate(channel, &handle, status);

  if (*status != 0) {
    if (port) {
      hal::SetLastErrorPreviouslyAllocated(status, "Relay", channel,
                                           port->previousAllocation);
    } else {
      hal::SetLastErrorIndexOutOfRange(status, "Invalid Index for Relay", 0,
                                       kNumRelayChannels, channel);
    }
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.
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
  port->previousAllocation = allocationLocation ? allocationLocation : "";

  return handle;
}

void HAL_FreeRelayPort(HAL_RelayHandle relayPortHandle) {
  auto port = relayHandles->Get(relayPortHandle);
  relayHandles->Free(relayPortHandle);
  if (port == nullptr) {
    return;
  }
  if (port->fwd) {
    SimRelayData[port->channel].initializedForward = false;
  } else {
    SimRelayData[port->channel].initializedReverse = false;
  }
}

HAL_Bool HAL_CheckRelayChannel(int32_t channel) {
  // roboRIO only has 4 headers, and the FPGA has
  // separate functions for forward and reverse,
  // instead of separate channel IDs
  return channel < kNumRelayHeaders && channel >= 0;
}

void HAL_SetRelay(HAL_RelayHandle relayPortHandle, HAL_Bool on,
                  int32_t* status) {
  auto port = relayHandles->Get(relayPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (port->fwd) {
    SimRelayData[port->channel].forward = on;
  } else {
    SimRelayData[port->channel].reverse = on;
  }
}

HAL_Bool HAL_GetRelay(HAL_RelayHandle relayPortHandle, int32_t* status) {
  auto port = relayHandles->Get(relayPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  if (port->fwd) {
    return SimRelayData[port->channel].forward;
  } else {
    return SimRelayData[port->channel].reverse;
  }
}
}  // extern "C"

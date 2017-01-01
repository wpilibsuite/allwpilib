/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Relay.h"

#include "DigitalInternal.h"
#include "HAL/handles/IndexedHandleResource.h"
#include "PortsInternal.h"

using namespace hal;

namespace {
struct Relay {
  uint8_t channel;
  bool fwd;
};
}

static IndexedHandleResource<HAL_RelayHandle, Relay, kNumRelayChannels,
                             HAL_HandleEnum::Relay>
    relayHandles;

// Create a mutex to protect changes to the relay values
static priority_recursive_mutex digitalRelayMutex;

extern "C" {
HAL_RelayHandle HAL_InitializeRelayPort(HAL_PortHandle portHandle, HAL_Bool fwd,
                                        int32_t* status) {
  initializeDigital(status);

  if (*status != 0) return HAL_kInvalidHandle;

  int16_t channel = getPortHandleChannel(portHandle);
  if (channel == InvalidHandleIndex) {
    *status = PARAMETER_OUT_OF_RANGE;
    return HAL_kInvalidHandle;
  }

  if (!fwd) channel += kNumRelayHeaders;  // add 4 to reverse channels

  auto handle = relayHandles.Allocate(channel, status);

  if (*status != 0)
    return HAL_kInvalidHandle;  // failed to allocate. Pass error back.

  auto port = relayHandles.Get(handle);
  if (port == nullptr) {  // would only occur on thread issue.
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }

  if (!fwd) {
    // Subtract number of headers to put channel in range
    channel -= kNumRelayHeaders;

    port->fwd = false;  // set to reverse
  } else {
    port->fwd = true;  // set to forward
  }

  port->channel = static_cast<uint8_t>(channel);
  return handle;
}

void HAL_FreeRelayPort(HAL_RelayHandle relayPortHandle) {
  // no status, so no need to check for a proper free.
  relayHandles.Free(relayPortHandle);
}

HAL_Bool HAL_CheckRelayChannel(int32_t channel) {
  // roboRIO only has 4 headers, and the FPGA has
  // seperate functions for forward and reverse,
  // instead of seperate channel IDs
  return channel < kNumRelayHeaders && channel >= 0;
}

/**
 * Set the state of a relay.
 * Set the state of a relay output.
 */
void HAL_SetRelay(HAL_RelayHandle relayPortHandle, HAL_Bool on,
                  int32_t* status) {
  auto port = relayHandles.Get(relayPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  std::lock_guard<priority_recursive_mutex> sync(digitalRelayMutex);
  uint8_t relays = 0;
  if (port->fwd) {
    relays = relaySystem->readValue_Forward(status);
  } else {
    relays = relaySystem->readValue_Reverse(status);
  }

  if (*status != 0) return;  // bad status read

  if (on) {
    relays |= 1 << port->channel;
  } else {
    relays &= ~(1 << port->channel);
  }

  if (port->fwd) {
    relaySystem->writeValue_Forward(relays, status);
  } else {
    relaySystem->writeValue_Reverse(relays, status);
  }
}

/**
 * Get the current state of the relay channel
 */
HAL_Bool HAL_GetRelay(HAL_RelayHandle relayPortHandle, int32_t* status) {
  auto port = relayHandles.Get(relayPortHandle);
  if (port == nullptr) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }

  uint8_t relays = 0;
  if (port->fwd) {
    relays = relaySystem->readValue_Forward(status);
  } else {
    relays = relaySystem->readValue_Reverse(status);
  }

  return (relays & (1 << port->channel)) != 0;
}
}

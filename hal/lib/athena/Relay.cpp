/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Relay.h"

#include "DigitalInternal.h"

static_assert(sizeof(uint32_t) <= sizeof(void*),
              "This file shoves uint32_ts into pointers.");

using namespace hal;

// Create a mutex to protect changes to the relay values
static priority_recursive_mutex digitalRelayMutex;

constexpr uint32_t kRelayPins = 8;

extern "C" {
bool checkRelayChannel(void* digital_port_pointer) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  return port->pin < kRelayPins;
}

/**
 * Check a port to make sure that it is not nullptr and is a valid Relay port.
 *
 * Sets the status to contain the appropriate error.
 *
 * @return true if the port passed validation.
 */
static bool verifyRelayChannel(DigitalPort* port, int32_t* status) {
  if (port == nullptr) {
    *status = NULL_PARAMETER;
    return false;
  } else if (!checkRelayChannel(port)) {
    *status = PARAMETER_OUT_OF_RANGE;
    return false;
  } else {
    return true;
  }
}

/**
 * Set the state of a relay.
 * Set the state of a relay output to be forward. Relays have two outputs and
 * each is
 * independently set to 0v or 12v.
 */
void setRelayForward(void* digital_port_pointer, bool on, int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  if (!verifyRelayChannel(port, status)) {
    return;
  }

  {
    std::lock_guard<priority_recursive_mutex> sync(digitalRelayMutex);
    uint8_t forwardRelays = relaySystem->readValue_Forward(status);
    if (on)
      forwardRelays |= 1 << port->pin;
    else
      forwardRelays &= ~(1 << port->pin);
    relaySystem->writeValue_Forward(forwardRelays, status);
  }
}

/**
 * Set the state of a relay.
 * Set the state of a relay output to be reverse. Relays have two outputs and
 * each is
 * independently set to 0v or 12v.
 */
void setRelayReverse(void* digital_port_pointer, bool on, int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  if (!verifyRelayChannel(port, status)) {
    return;
  }

  {
    std::lock_guard<priority_recursive_mutex> sync(digitalRelayMutex);
    uint8_t reverseRelays = relaySystem->readValue_Reverse(status);
    if (on)
      reverseRelays |= 1 << port->pin;
    else
      reverseRelays &= ~(1 << port->pin);
    relaySystem->writeValue_Reverse(reverseRelays, status);
  }
}

/**
 * Get the current state of the forward relay channel
 */
bool getRelayForward(void* digital_port_pointer, int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  if (!verifyRelayChannel(port, status)) {
    return false;
  }

  uint8_t forwardRelays = relaySystem->readValue_Forward(status);
  return (forwardRelays & (1 << port->pin)) != 0;
}

/**
 * Get the current state of the reverse relay channel
 */
bool getRelayReverse(void* digital_port_pointer, int32_t* status) {
  DigitalPort* port = (DigitalPort*)digital_port_pointer;
  if (!verifyRelayChannel(port, status)) {
    return false;
  }

  uint8_t reverseRelays = relaySystem->readValue_Reverse(status);
  return (reverseRelays & (1 << port->pin)) != 0;
}
}

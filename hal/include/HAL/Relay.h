/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "Handles.h"

extern "C" {
HalRelayHandle initializeRelayPort(HalPortHandle port_handle, uint8_t fwd,
                                   int32_t* status);
void freeRelayPort(HalRelayHandle relay_port_handle);

bool checkRelayChannel(uint8_t pin);

void setRelay(HalRelayHandle relay_port_handle, bool on, int32_t* status);
bool getRelay(HalRelayHandle relay_port_handle, int32_t* status);
}

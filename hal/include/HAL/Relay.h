/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

extern "C" {
bool checkRelayChannel(void* digital_port_pointer);

void setRelayForward(void* digital_port_pointer, bool on, int32_t* status);
void setRelayReverse(void* digital_port_pointer, bool on, int32_t* status);
bool getRelayForward(void* digital_port_pointer, int32_t* status);
bool getRelayReverse(void* digital_port_pointer, int32_t* status);
}

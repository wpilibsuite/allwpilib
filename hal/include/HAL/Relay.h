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
HAL_RelayHandle HAL_InitializeRelayPort(HAL_PortHandle port_handle, uint8_t fwd,
                                        int32_t* status);
void HAL_FreeRelayPort(HAL_RelayHandle relay_port_handle);

bool HAL_CheckRelayChannel(uint8_t pin);

void HAL_SetRelay(HAL_RelayHandle relay_port_handle, bool on, int32_t* status);
bool HAL_GetRelay(HAL_RelayHandle relay_port_handle, int32_t* status);
}

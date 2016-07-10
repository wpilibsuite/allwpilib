/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "Types.h"

extern "C" {
HAL_RelayHandle HAL_InitializeRelayPort(HAL_PortHandle port_handle,
                                        HAL_Bool fwd, int32_t* status);
void HAL_FreeRelayPort(HAL_RelayHandle relay_port_handle);

HAL_Bool HAL_CheckRelayChannel(int32_t pin);

void HAL_SetRelay(HAL_RelayHandle relay_port_handle, HAL_Bool on,
                  int32_t* status);
HAL_Bool HAL_GetRelay(HAL_RelayHandle relay_port_handle, int32_t* status);
}

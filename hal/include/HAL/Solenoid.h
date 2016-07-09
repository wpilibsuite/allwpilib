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
HAL_SolenoidHandle HAL_InitializeSolenoidPort(HAL_PortHandle port_handle,
                                              int32_t* status);
void HAL_FreeSolenoidPort(HAL_SolenoidHandle solenoid_port_handle);
bool HAL_CheckSolenoidModule(uint8_t module);

bool HAL_GetSolenoid(HAL_SolenoidHandle solenoid_port_handle, int32_t* status);
uint8_t HAL_GetAllSolenoids(uint8_t module, int32_t* status);
void HAL_SetSolenoid(HAL_SolenoidHandle solenoid_port_handle, bool value,
                     int32_t* status);

int HAL_GetPCMSolenoidBlackList(uint8_t module, int32_t* status);
bool HAL_GetPCMSolenoidVoltageStickyFault(uint8_t module, int32_t* status);
bool HAL_GetPCMSolenoidVoltageFault(uint8_t module, int32_t* status);
void HAL_ClearAllPCMStickyFaults(uint8_t module, int32_t* status);
}

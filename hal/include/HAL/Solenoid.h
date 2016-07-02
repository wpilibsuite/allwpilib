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
HalSolenoidHandle initializeSolenoidPort(HalPortHandle port_handle,
                                         int32_t* status);
void freeSolenoidPort(HalSolenoidHandle solenoid_port_handle);
bool checkSolenoidModule(uint8_t module);

bool getSolenoid(HalSolenoidHandle solenoid_port_handle, int32_t* status);
uint8_t getAllSolenoids(uint8_t module, int32_t* status);
void setSolenoid(HalSolenoidHandle solenoid_port_handle, bool value,
                 int32_t* status);

int getPCMSolenoidBlackList(uint8_t module, int32_t* status);
bool getPCMSolenoidVoltageStickyFault(uint8_t module, int32_t* status);
bool getPCMSolenoidVoltageFault(uint8_t module, int32_t* status);
void clearAllPCMStickyFaults_sol(uint8_t module, int32_t* status);
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

extern "C" {
void initializePDP(uint8_t module);
double getPDPTemperature(uint8_t module, int32_t* status);
double getPDPVoltage(uint8_t module, int32_t* status);
double getPDPChannelCurrent(uint8_t module, uint8_t channel, int32_t* status);
double getPDPTotalCurrent(uint8_t module, int32_t* status);
double getPDPTotalPower(uint8_t module, int32_t* status);
double getPDPTotalEnergy(uint8_t module, int32_t* status);
void resetPDPTotalEnergy(uint8_t module, int32_t* status);
void clearPDPStickyFaults(uint8_t module, int32_t* status);
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <stdint.h>

extern "C" {
bool HALInitialize() { return true; }

uint32_t HALReport(uint8_t resource, uint8_t instanceNumber,
                   uint8_t context, const char* feature) { return 0; }

bool checkRelayChannel(uint8_t pin) { return true; }
}

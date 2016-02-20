/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

extern "C" {
bool checkPWMChannel(void* digital_port_pointer);

void setPWM(void* digital_port_pointer, unsigned short value, int32_t* status);
bool allocatePWMChannel(void* digital_port_pointer, int32_t* status);
void freePWMChannel(void* digital_port_pointer, int32_t* status);
unsigned short getPWM(void* digital_port_pointer, int32_t* status);
void latchPWMZero(void* digital_port_pointer, int32_t* status);
void setPWMPeriodScale(void* digital_port_pointer, uint32_t squelchMask,
                       int32_t* status);
uint16_t getLoopTiming(int32_t* status);
}

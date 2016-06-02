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
void* initializeAnalogOutputPort(HalPortHandle port_handle, int32_t* status);
void freeAnalogOutputPort(void* analog_port_pointer);
void setAnalogOutput(void* analog_port_pointer, double voltage,
                     int32_t* status);
double getAnalogOutput(void* analog_port_pointer, int32_t* status);
bool checkAnalogOutputChannel(uint32_t pin);
}

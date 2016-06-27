/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "Handles.h"

typedef HalHandle HalAnalogOutputHandle;

extern "C" {
HalAnalogOutputHandle initializeAnalogOutputPort(HalPortHandle port_handle,
                                                 int32_t* status);
void freeAnalogOutputPort(HalAnalogOutputHandle analog_output_handle);
void setAnalogOutput(HalAnalogOutputHandle analog_output_handle, double voltage,
                     int32_t* status);
double getAnalogOutput(HalAnalogOutputHandle analog_output_handle,
                       int32_t* status);
bool checkAnalogOutputChannel(uint32_t pin);
}

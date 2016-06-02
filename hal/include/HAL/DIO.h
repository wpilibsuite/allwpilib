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
// the following 2 functions are here as they will be changed with
// the handle changes to be DIO exclusive.
void* initializeDigitalPort(HalPortHandle port_handle, int32_t* status);
void freeDigitalPort(void* digital_port_pointer);

void* allocatePWM(int32_t* status);
void freePWM(void* pwmGenerator, int32_t* status);
void setPWMRate(double rate, int32_t* status);
void setPWMDutyCycle(void* pwmGenerator, double dutyCycle, int32_t* status);
void setPWMOutputChannel(void* pwmGenerator, uint32_t pin, int32_t* status);

bool allocateDIO(void* digital_port_pointer, bool input, int32_t* status);
void freeDIO(void* digital_port_pointer, int32_t* status);
void setDIO(void* digital_port_pointer, short value, int32_t* status);
bool getDIO(void* digital_port_pointer, int32_t* status);
bool getDIODirection(void* digital_port_pointer, int32_t* status);
void pulse(void* digital_port_pointer, double pulseLength, int32_t* status);
bool isPulsing(void* digital_port_pointer, int32_t* status);
bool isAnyPulsing(int32_t* status);

void setFilterSelect(void* digital_port_pointer, int filter_index,
                     int32_t* status);
int getFilterSelect(void* digital_port_pointer, int32_t* status);

void setFilterPeriod(int filter_index, uint32_t value, int32_t* status);
uint32_t getFilterPeriod(int filter_index, int32_t* status);
}

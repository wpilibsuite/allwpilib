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
HalDigitalHandle initializeDIOPort(HalPortHandle port_handle, uint8_t input,
                                   int32_t* status);
void freeDIOPort(HalDigitalHandle dio_port_handle);

void* allocatePWM(int32_t* status);
void freePWM(void* pwmGenerator, int32_t* status);
void setPWMRate(double rate, int32_t* status);
void setPWMDutyCycle(void* pwmGenerator, double dutyCycle, int32_t* status);
void setPWMOutputChannel(void* pwmGenerator, uint32_t pin, int32_t* status);

void setDIO(HalDigitalHandle dio_port_handle, short value, int32_t* status);
bool getDIO(HalDigitalHandle dio_port_handle, int32_t* status);
bool getDIODirection(HalDigitalHandle dio_port_handle, int32_t* status);
void pulse(HalDigitalHandle dio_port_handle, double pulseLength,
           int32_t* status);
bool isPulsing(HalDigitalHandle dio_port_handle, int32_t* status);
bool isAnyPulsing(int32_t* status);

void setFilterSelect(HalDigitalHandle dio_port_handle, int filter_index,
                     int32_t* status);
int getFilterSelect(HalDigitalHandle dio_port_handle, int32_t* status);

void setFilterPeriod(int filter_index, uint32_t value, int32_t* status);
uint32_t getFilterPeriod(int filter_index, int32_t* status);
}

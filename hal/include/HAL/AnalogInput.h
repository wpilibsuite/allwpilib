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
// Analog input functions
void* initializeAnalogInputPort(HalPortHandle port_handle, int32_t* status);
void freeAnalogInputPort(void* analog_port_pointer);
bool checkAnalogModule(uint8_t module);
bool checkAnalogInputChannel(uint32_t pin);

void setAnalogSampleRate(double samplesPerSecond, int32_t* status);
float getAnalogSampleRate(int32_t* status);
void setAnalogAverageBits(void* analog_port_pointer, uint32_t bits,
                          int32_t* status);
uint32_t getAnalogAverageBits(void* analog_port_pointer, int32_t* status);
void setAnalogOversampleBits(void* analog_port_pointer, uint32_t bits,
                             int32_t* status);
uint32_t getAnalogOversampleBits(void* analog_port_pointer, int32_t* status);
int16_t getAnalogValue(void* analog_port_pointer, int32_t* status);
int32_t getAnalogAverageValue(void* analog_port_pointer, int32_t* status);
int32_t getAnalogVoltsToValue(void* analog_port_pointer, double voltage,
                              int32_t* status);
float getAnalogVoltage(void* analog_port_pointer, int32_t* status);
float getAnalogAverageVoltage(void* analog_port_pointer, int32_t* status);
uint32_t getAnalogLSBWeight(void* analog_port_pointer, int32_t* status);
int32_t getAnalogOffset(void* analog_port_pointer, int32_t* status);
}

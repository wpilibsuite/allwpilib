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
HalAnalogInputHandle initializeAnalogInputPort(HalPortHandle port_handle,
                                               int32_t* status);
void freeAnalogInputPort(HalAnalogInputHandle analog_port_handle);
bool checkAnalogModule(uint8_t module);
bool checkAnalogInputChannel(uint32_t pin);

void setAnalogSampleRate(double samplesPerSecond, int32_t* status);
float getAnalogSampleRate(int32_t* status);
void setAnalogAverageBits(HalAnalogInputHandle analog_port_handle,
                          uint32_t bits, int32_t* status);
uint32_t getAnalogAverageBits(HalAnalogInputHandle analog_port_handle,
                              int32_t* status);
void setAnalogOversampleBits(HalAnalogInputHandle analog_port_handle,
                             uint32_t bits, int32_t* status);
uint32_t getAnalogOversampleBits(HalAnalogInputHandle analog_port_handle,
                                 int32_t* status);
int16_t getAnalogValue(HalAnalogInputHandle analog_port_handle,
                       int32_t* status);
int32_t getAnalogAverageValue(HalAnalogInputHandle analog_port_handle,
                              int32_t* status);
int32_t getAnalogVoltsToValue(HalAnalogInputHandle analog_port_handle,
                              double voltage, int32_t* status);
float getAnalogVoltage(HalAnalogInputHandle analog_port_handle,
                       int32_t* status);
float getAnalogAverageVoltage(HalAnalogInputHandle analog_port_handle,
                              int32_t* status);
uint32_t getAnalogLSBWeight(HalAnalogInputHandle analog_port_handle,
                            int32_t* status);
int32_t getAnalogOffset(HalAnalogInputHandle analog_port_handle,
                        int32_t* status);
}

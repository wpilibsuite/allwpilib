#pragma once
#include <stdint.h>

extern "C" {
void* initializeAnalogOutputPort(void* port_pointer, int32_t* status);
void freeAnalogOutputPort(void* analog_port_pointer);
void setAnalogOutput(void* analog_port_pointer, double voltage,
                     int32_t* status);
double getAnalogOutput(void* analog_port_pointer, int32_t* status);
bool checkAnalogOutputChannel(uint32_t pin);
}

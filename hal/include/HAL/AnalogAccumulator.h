#pragma once
#include <stdint.h>

extern "C" {
bool isAccumulatorChannel(void* analog_port_pointer, int32_t* status);
void initAccumulator(void* analog_port_pointer, int32_t* status);
void resetAccumulator(void* analog_port_pointer, int32_t* status);
void setAccumulatorCenter(void* analog_port_pointer, int32_t center,
                          int32_t* status);
void setAccumulatorDeadband(void* analog_port_pointer, int32_t deadband,
                            int32_t* status);
int64_t getAccumulatorValue(void* analog_port_pointer, int32_t* status);
uint32_t getAccumulatorCount(void* analog_port_pointer, int32_t* status);
void getAccumulatorOutput(void* analog_port_pointer, int64_t* value,
                          uint32_t* count, int32_t* status);
}

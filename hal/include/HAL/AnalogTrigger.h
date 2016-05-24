#pragma once
#include <stdint.h>

enum AnalogTriggerType {
  kInWindow = 0,
  kState = 1,
  kRisingPulse = 2,
  kFallingPulse = 3
};

extern "C" {
void* initializeAnalogTrigger(void* port_pointer, uint32_t* index,
                              int32_t* status);
void cleanAnalogTrigger(void* analog_trigger_pointer, int32_t* status);
void setAnalogTriggerLimitsRaw(void* analog_trigger_pointer, int32_t lower,
                               int32_t upper, int32_t* status);
void setAnalogTriggerLimitsVoltage(void* analog_trigger_pointer, double lower,
                                   double upper, int32_t* status);
void setAnalogTriggerAveraged(void* analog_trigger_pointer,
                              bool useAveragedValue, int32_t* status);
void setAnalogTriggerFiltered(void* analog_trigger_pointer,
                              bool useFilteredValue, int32_t* status);
bool getAnalogTriggerInWindow(void* analog_trigger_pointer, int32_t* status);
bool getAnalogTriggerTriggerState(void* analog_trigger_pointer,
                                  int32_t* status);
bool getAnalogTriggerOutput(void* analog_trigger_pointer,
                            AnalogTriggerType type, int32_t* status);
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "Handles.h"

enum AnalogTriggerType {
  kInWindow = 0,
  kState = 1,
  kRisingPulse = 2,
  kFallingPulse = 3
};

extern "C" {
HalAnalogTriggerHandle initializeAnalogTrigger(HalAnalogInputHandle port_handle,
                                               uint32_t* index,
                                               int32_t* status);
void cleanAnalogTrigger(HalAnalogTriggerHandle analog_trigger_handle,
                        int32_t* status);
void setAnalogTriggerLimitsRaw(HalAnalogTriggerHandle analog_trigger_handle,
                               int32_t lower, int32_t upper, int32_t* status);
void setAnalogTriggerLimitsVoltage(HalAnalogTriggerHandle analog_trigger_handle,
                                   double lower, double upper, int32_t* status);
void setAnalogTriggerAveraged(HalAnalogTriggerHandle analog_trigger_handle,
                              bool useAveragedValue, int32_t* status);
void setAnalogTriggerFiltered(HalAnalogTriggerHandle analog_trigger_handle,
                              bool useFilteredValue, int32_t* status);
bool getAnalogTriggerInWindow(HalAnalogTriggerHandle analog_trigger_handle,
                              int32_t* status);
bool getAnalogTriggerTriggerState(HalAnalogTriggerHandle analog_trigger_handle,
                                  int32_t* status);
bool getAnalogTriggerOutput(HalAnalogTriggerHandle analog_trigger_handle,
                            AnalogTriggerType type, int32_t* status);
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/Handles.h"

enum Mode {
  kTwoPulse = 0,
  kSemiperiod = 1,
  kPulseLength = 2,
  kExternalDirection = 3
};

extern "C" {
HalCounterHandle initializeCounter(Mode mode, uint32_t* index, int32_t* status);
void freeCounter(HalCounterHandle counter_handle, int32_t* status);
void setCounterAverageSize(HalCounterHandle counter_handle, int32_t size,
                           int32_t* status);
void setCounterUpSource(HalCounterHandle counter_handle, uint32_t pin,
                        bool analogTrigger, int32_t* status);
void setCounterUpSourceEdge(HalCounterHandle counter_handle, bool risingEdge,
                            bool fallingEdge, int32_t* status);
void clearCounterUpSource(HalCounterHandle counter_handle, int32_t* status);
void setCounterDownSource(HalCounterHandle counter_handle, uint32_t pin,
                          bool analogTrigger, int32_t* status);
void setCounterDownSourceEdge(HalCounterHandle counter_handle, bool risingEdge,
                              bool fallingEdge, int32_t* status);
void clearCounterDownSource(HalCounterHandle counter_handle, int32_t* status);
void setCounterUpDownMode(HalCounterHandle counter_handle, int32_t* status);
void setCounterExternalDirectionMode(HalCounterHandle counter_handle,
                                     int32_t* status);
void setCounterSemiPeriodMode(HalCounterHandle counter_handle,
                              bool highSemiPeriod, int32_t* status);
void setCounterPulseLengthMode(HalCounterHandle counter_handle,
                               double threshold, int32_t* status);
int32_t getCounterSamplesToAverage(HalCounterHandle counter_handle,
                                   int32_t* status);
void setCounterSamplesToAverage(HalCounterHandle counter_handle,
                                int samplesToAverage, int32_t* status);
void resetCounter(HalCounterHandle counter_handle, int32_t* status);
int32_t getCounter(HalCounterHandle counter_handle, int32_t* status);
double getCounterPeriod(HalCounterHandle counter_handle, int32_t* status);
void setCounterMaxPeriod(HalCounterHandle counter_handle, double maxPeriod,
                         int32_t* status);
void setCounterUpdateWhenEmpty(HalCounterHandle counter_handle, bool enabled,
                               int32_t* status);
bool getCounterStopped(HalCounterHandle counter_handle, int32_t* status);
bool getCounterDirection(HalCounterHandle counter_handle, int32_t* status);
void setCounterReverseDirection(HalCounterHandle counter_handle,
                                bool reverseDirection, int32_t* status);
}

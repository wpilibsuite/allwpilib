/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

enum Mode {
  kTwoPulse = 0,
  kSemiperiod = 1,
  kPulseLength = 2,
  kExternalDirection = 3
};

extern "C" {
void* initializeCounter(Mode mode, uint32_t* index, int32_t* status);
void freeCounter(void* counter_pointer, int32_t* status);
void setCounterAverageSize(void* counter_pointer, int32_t size,
                           int32_t* status);
void setCounterUpSource(void* counter_pointer, uint32_t pin, bool analogTrigger,
                        int32_t* status);
void setCounterUpSourceEdge(void* counter_pointer, bool risingEdge,
                            bool fallingEdge, int32_t* status);
void clearCounterUpSource(void* counter_pointer, int32_t* status);
void setCounterDownSource(void* counter_pointer, uint32_t pin,
                          bool analogTrigger, int32_t* status);
void setCounterDownSourceEdge(void* counter_pointer, bool risingEdge,
                              bool fallingEdge, int32_t* status);
void clearCounterDownSource(void* counter_pointer, int32_t* status);
void setCounterUpDownMode(void* counter_pointer, int32_t* status);
void setCounterExternalDirectionMode(void* counter_pointer, int32_t* status);
void setCounterSemiPeriodMode(void* counter_pointer, bool highSemiPeriod,
                              int32_t* status);
void setCounterPulseLengthMode(void* counter_pointer, double threshold,
                               int32_t* status);
int32_t getCounterSamplesToAverage(void* counter_pointer, int32_t* status);
void setCounterSamplesToAverage(void* counter_pointer, int samplesToAverage,
                                int32_t* status);
void resetCounter(void* counter_pointer, int32_t* status);
int32_t getCounter(void* counter_pointer, int32_t* status);
double getCounterPeriod(void* counter_pointer, int32_t* status);
void setCounterMaxPeriod(void* counter_pointer, double maxPeriod,
                         int32_t* status);
void setCounterUpdateWhenEmpty(void* counter_pointer, bool enabled,
                               int32_t* status);
bool getCounterStopped(void* counter_pointer, int32_t* status);
bool getCounterDirection(void* counter_pointer, int32_t* status);
void setCounterReverseDirection(void* counter_pointer, bool reverseDirection,
                                int32_t* status);
}

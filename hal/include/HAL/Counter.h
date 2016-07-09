/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/AnalogTrigger.h"
#include "HAL/Handles.h"

enum HAL_Counter_Mode {
  HAL_Counter_kTwoPulse = 0,
  HAL_Counter_kSemiperiod = 1,
  HAL_Counter_kPulseLength = 2,
  HAL_Counter_kExternalDirection = 3
};

extern "C" {
HAL_CounterHandle HAL_InitializeCounter(HAL_Counter_Mode mode, int32_t* index,
                                        int32_t* status);
void HAL_FreeCounter(HAL_CounterHandle counter_handle, int32_t* status);
void HAL_SetCounterAverageSize(HAL_CounterHandle counter_handle, int32_t size,
                               int32_t* status);
void HAL_SetCounterUpSource(HAL_CounterHandle counter_handle,
                            HAL_Handle digitalSourceHandle,
                            HAL_AnalogTriggerType analogTriggerType,
                            int32_t* status);
void HAL_SetCounterUpSourceEdge(HAL_CounterHandle counter_handle,
                                bool risingEdge, bool fallingEdge,
                                int32_t* status);
void HAL_ClearCounterUpSource(HAL_CounterHandle counter_handle,
                              int32_t* status);
void HAL_SetCounterDownSource(HAL_CounterHandle counter_handle,
                              HAL_Handle digitalSourceHandle,
                              HAL_AnalogTriggerType analogTriggerType,
                              int32_t* status);
void HAL_SetCounterDownSourceEdge(HAL_CounterHandle counter_handle,
                                  bool risingEdge, bool fallingEdge,
                                  int32_t* status);
void HAL_ClearCounterDownSource(HAL_CounterHandle counter_handle,
                                int32_t* status);
void HAL_SetCounterUpDownMode(HAL_CounterHandle counter_handle,
                              int32_t* status);
void HAL_SetCounterExternalDirectionMode(HAL_CounterHandle counter_handle,
                                         int32_t* status);
void HAL_SetCounterSemiPeriodMode(HAL_CounterHandle counter_handle,
                                  bool highSemiPeriod, int32_t* status);
void HAL_SetCounterPulseLengthMode(HAL_CounterHandle counter_handle,
                                   double threshold, int32_t* status);
int32_t HAL_GetCounterSamplesToAverage(HAL_CounterHandle counter_handle,
                                       int32_t* status);
void HAL_SetCounterSamplesToAverage(HAL_CounterHandle counter_handle,
                                    int samplesToAverage, int32_t* status);
void HAL_ResetCounter(HAL_CounterHandle counter_handle, int32_t* status);
int32_t HAL_GetCounter(HAL_CounterHandle counter_handle, int32_t* status);
double HAL_GetCounterPeriod(HAL_CounterHandle counter_handle, int32_t* status);
void HAL_SetCounterMaxPeriod(HAL_CounterHandle counter_handle, double maxPeriod,
                             int32_t* status);
void HAL_SetCounterUpdateWhenEmpty(HAL_CounterHandle counter_handle,
                                   bool enabled, int32_t* status);
bool HAL_GetCounterStopped(HAL_CounterHandle counter_handle, int32_t* status);
bool HAL_GetCounterDirection(HAL_CounterHandle counter_handle, int32_t* status);
void HAL_SetCounterReverseDirection(HAL_CounterHandle counter_handle,
                                    bool reverseDirection, int32_t* status);
}

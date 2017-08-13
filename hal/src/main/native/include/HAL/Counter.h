/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/AnalogTrigger.h"
#include "HAL/Types.h"

enum HAL_Counter_Mode : int32_t {
  HAL_Counter_kTwoPulse = 0,
  HAL_Counter_kSemiperiod = 1,
  HAL_Counter_kPulseLength = 2,
  HAL_Counter_kExternalDirection = 3
};

#ifdef __cplusplus
extern "C" {
#endif
HAL_CounterHandle HAL_InitializeCounter(HAL_Counter_Mode mode, int32_t* index,
                                        int32_t* status);
void HAL_FreeCounter(HAL_CounterHandle counterHandle, int32_t* status);
void HAL_SetCounterAverageSize(HAL_CounterHandle counterHandle, int32_t size,
                               int32_t* status);
void HAL_SetCounterUpSource(HAL_CounterHandle counterHandle,
                            HAL_Handle digitalSourceHandle,
                            HAL_AnalogTriggerType analogTriggerType,
                            int32_t* status);
void HAL_SetCounterUpSourceEdge(HAL_CounterHandle counterHandle,
                                HAL_Bool risingEdge, HAL_Bool fallingEdge,
                                int32_t* status);
void HAL_ClearCounterUpSource(HAL_CounterHandle counterHandle, int32_t* status);
void HAL_SetCounterDownSource(HAL_CounterHandle counterHandle,
                              HAL_Handle digitalSourceHandle,
                              HAL_AnalogTriggerType analogTriggerType,
                              int32_t* status);
void HAL_SetCounterDownSourceEdge(HAL_CounterHandle counterHandle,
                                  HAL_Bool risingEdge, HAL_Bool fallingEdge,
                                  int32_t* status);
void HAL_ClearCounterDownSource(HAL_CounterHandle counterHandle,
                                int32_t* status);
void HAL_SetCounterUpDownMode(HAL_CounterHandle counterHandle, int32_t* status);
void HAL_SetCounterExternalDirectionMode(HAL_CounterHandle counterHandle,
                                         int32_t* status);
void HAL_SetCounterSemiPeriodMode(HAL_CounterHandle counterHandle,
                                  HAL_Bool highSemiPeriod, int32_t* status);
void HAL_SetCounterPulseLengthMode(HAL_CounterHandle counterHandle,
                                   double threshold, int32_t* status);
int32_t HAL_GetCounterSamplesToAverage(HAL_CounterHandle counterHandle,
                                       int32_t* status);
void HAL_SetCounterSamplesToAverage(HAL_CounterHandle counterHandle,
                                    int32_t samplesToAverage, int32_t* status);
void HAL_ResetCounter(HAL_CounterHandle counterHandle, int32_t* status);
int32_t HAL_GetCounter(HAL_CounterHandle counterHandle, int32_t* status);
double HAL_GetCounterPeriod(HAL_CounterHandle counterHandle, int32_t* status);
void HAL_SetCounterMaxPeriod(HAL_CounterHandle counterHandle, double maxPeriod,
                             int32_t* status);
void HAL_SetCounterUpdateWhenEmpty(HAL_CounterHandle counterHandle,
                                   HAL_Bool enabled, int32_t* status);
HAL_Bool HAL_GetCounterStopped(HAL_CounterHandle counterHandle,
                               int32_t* status);
HAL_Bool HAL_GetCounterDirection(HAL_CounterHandle counterHandle,
                                 int32_t* status);
void HAL_SetCounterReverseDirection(HAL_CounterHandle counterHandle,
                                    HAL_Bool reverseDirection, int32_t* status);
#ifdef __cplusplus
}
#endif

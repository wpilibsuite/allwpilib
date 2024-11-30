// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Counter.h"

#include <limits>
#include <memory>

#include <fmt/format.h>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/HAL.h"
#include "hal/handles/LimitedHandleResource.h"

using namespace hal;

namespace hal::init {
void InitializeCounter() {}
}  // namespace hal::init

extern "C" {

HAL_CounterHandle HAL_InitializeCounter(HAL_Counter_Mode mode, int32_t* index,
                                        int32_t* status) {
  hal::init::CheckInit();
  *status = HAL_HANDLE_ERROR;
  return HAL_kInvalidHandle;
}

void HAL_FreeCounter(HAL_CounterHandle counterHandle) {}

void HAL_SetCounterAverageSize(HAL_CounterHandle counterHandle, int32_t size,
                               int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetCounterUpSource(HAL_CounterHandle counterHandle,
                            HAL_Handle digitalSourceHandle,
                            HAL_AnalogTriggerType analogTriggerType,
                            int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetCounterUpSourceEdge(HAL_CounterHandle counterHandle,
                                HAL_Bool risingEdge, HAL_Bool fallingEdge,
                                int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_ClearCounterUpSource(HAL_CounterHandle counterHandle,
                              int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetCounterDownSource(HAL_CounterHandle counterHandle,
                              HAL_Handle digitalSourceHandle,
                              HAL_AnalogTriggerType analogTriggerType,
                              int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetCounterDownSourceEdge(HAL_CounterHandle counterHandle,
                                  HAL_Bool risingEdge, HAL_Bool fallingEdge,
                                  int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_ClearCounterDownSource(HAL_CounterHandle counterHandle,
                                int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetCounterUpDownMode(HAL_CounterHandle counterHandle,
                              int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetCounterExternalDirectionMode(HAL_CounterHandle counterHandle,
                                         int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetCounterSemiPeriodMode(HAL_CounterHandle counterHandle,
                                  HAL_Bool highSemiPeriod, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetCounterPulseLengthMode(HAL_CounterHandle counterHandle,
                                   double threshold, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

int32_t HAL_GetCounterSamplesToAverage(HAL_CounterHandle counterHandle,
                                       int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_SetCounterSamplesToAverage(HAL_CounterHandle counterHandle,
                                    int32_t samplesToAverage, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_ResetCounter(HAL_CounterHandle counterHandle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

int32_t HAL_GetCounter(HAL_CounterHandle counterHandle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

double HAL_GetCounterPeriod(HAL_CounterHandle counterHandle, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_SetCounterMaxPeriod(HAL_CounterHandle counterHandle, double maxPeriod,
                             int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetCounterUpdateWhenEmpty(HAL_CounterHandle counterHandle,
                                   HAL_Bool enabled, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

HAL_Bool HAL_GetCounterStopped(HAL_CounterHandle counterHandle,
                               int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return false;
}

HAL_Bool HAL_GetCounterDirection(HAL_CounterHandle counterHandle,
                                 int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return false;
}

void HAL_SetCounterReverseDirection(HAL_CounterHandle counterHandle,
                                    HAL_Bool reverseDirection,
                                    int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

}  // extern "C"

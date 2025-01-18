// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Interrupts.h"

#include <memory>

#include <wpi/SafeThread.h>

#include "HALInitializer.h"
#include "HALInternal.h"
#include "PortsInternal.h"
#include "hal/Errors.h"
#include "hal/HALBase.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"

using namespace hal;

namespace hal::init {
void InitializeInterrupts() {}
}  // namespace hal::init

extern "C" {

HAL_InterruptHandle HAL_InitializeInterrupts(int32_t* status) {
  hal::init::CheckInit();
  *status = HAL_HANDLE_ERROR;
  return HAL_kInvalidHandle;
}

void HAL_CleanInterrupts(HAL_InterruptHandle interruptHandle) {}

int64_t HAL_WaitForInterrupt(HAL_InterruptHandle interruptHandle,
                             double timeout, HAL_Bool ignorePrevious,
                             int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int64_t HAL_WaitForMultipleInterrupts(HAL_InterruptHandle interruptHandle,
                                      int64_t mask, double timeout,
                                      HAL_Bool ignorePrevious,
                                      int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int64_t HAL_ReadInterruptRisingTimestamp(HAL_InterruptHandle interruptHandle,
                                         int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

int64_t HAL_ReadInterruptFallingTimestamp(HAL_InterruptHandle interruptHandle,
                                          int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return 0;
}

void HAL_RequestInterrupts(HAL_InterruptHandle interruptHandle,
                           HAL_Handle digitalSourceHandle,
                           HAL_AnalogTriggerType analogTriggerType,
                           int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_SetInterruptUpSourceEdge(HAL_InterruptHandle interruptHandle,
                                  HAL_Bool risingEdge, HAL_Bool fallingEdge,
                                  int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

void HAL_ReleaseWaitingInterrupt(HAL_InterruptHandle interruptHandle,
                                 int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

}  // extern "C"

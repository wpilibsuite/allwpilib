/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "HAL/AnalogTrigger.h"
#include "HAL/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*HAL_InterruptHandlerFunction)(uint32_t interruptAssertedMask,
                                             void* param);

HAL_InterruptHandle HAL_InitializeInterrupts(HAL_Bool watcher, int32_t* status);
void* HAL_CleanInterrupts(HAL_InterruptHandle interruptHandle, int32_t* status);

/**
 * In synchronous mode, wait for the defined interrupt to occur.
 * @param timeout Timeout in seconds
 * @param ignorePrevious If true, ignore interrupts that happened before
 * waitForInterrupt was called.
 * @return The mask of interrupts that fired.
 */
int64_t HAL_WaitForInterrupt(HAL_InterruptHandle interruptHandle,
                             double timeout, HAL_Bool ignorePrevious,
                             int32_t* status);

/**
 * Enable interrupts to occur on this input.
 * Interrupts are disabled when the RequestInterrupt call is made. This gives
 * time to do the setup of the other options before starting to field
 * interrupts.
 */
void HAL_EnableInterrupts(HAL_InterruptHandle interruptHandle, int32_t* status);

/**
 * Disable Interrupts without without deallocating structures.
 */
void HAL_DisableInterrupts(HAL_InterruptHandle interruptHandle,
                           int32_t* status);

/**
 * Return the timestamp for the rising interrupt that occurred most recently.
 * This is in the same time domain as GetClock().
 * @return Timestamp in seconds since boot.
 */
double HAL_ReadInterruptRisingTimestamp(HAL_InterruptHandle interruptHandle,
                                        int32_t* status);

/**
 * Return the timestamp for the falling interrupt that occurred most recently.
 * This is in the same time domain as GetClock().
 * @return Timestamp in seconds since boot.
 */
double HAL_ReadInterruptFallingTimestamp(HAL_InterruptHandle interruptHandle,
                                         int32_t* status);

void HAL_RequestInterrupts(HAL_InterruptHandle interruptHandle,
                           HAL_Handle digitalSourceHandle,
                           HAL_AnalogTriggerType analogTriggerType,
                           int32_t* status);
void HAL_AttachInterruptHandler(HAL_InterruptHandle interruptHandle,
                                HAL_InterruptHandlerFunction handler,
                                void* param, int32_t* status);
void HAL_AttachInterruptHandlerThreaded(HAL_InterruptHandle interruptHandle,
                                        HAL_InterruptHandlerFunction handler,
                                        void* param, int32_t* status);
void HAL_SetInterruptUpSourceEdge(HAL_InterruptHandle interruptHandle,
                                  HAL_Bool risingEdge, HAL_Bool fallingEdge,
                                  int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/AnalogTrigger.h"
#include "hal/Types.h"

/**
 * @defgroup hal_interrupts Interrupts Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes an interrupt.
 *
 * @param[out] status Error status variable. 0 on success.
 * @return the created interrupt handle
 */
HAL_InterruptHandle HAL_InitializeInterrupts(int32_t* status);

/**
 * Frees an interrupt.
 *
 * @param interruptHandle the interrupt handle
 */
void HAL_CleanInterrupts(HAL_InterruptHandle interruptHandle);

/**
 * Waits for the defined interrupt to occur.
 *
 * @param[in] interruptHandle the interrupt handle
 * @param[in] timeout         timeout in seconds
 * @param[in] ignorePrevious  if true, ignore interrupts that happened before
 *                            waitForInterrupt was called
 * @param[out] status         Error status variable. 0 on success.
 * @return the mask of interrupts that fired
 */
int64_t HAL_WaitForInterrupt(HAL_InterruptHandle interruptHandle,
                             double timeout, HAL_Bool ignorePrevious,
                             int32_t* status);

/**
 * Waits for any interrupt covered by the mask to occur.
 *
 * @param[in] interruptHandle the interrupt handle to use for the context
 * @param[in] mask            the mask of interrupts to wait for
 * @param[in] timeout         timeout in seconds
 * @param[in] ignorePrevious  if true, ignore interrupts that happened before
 *                            waitForInterrupt was called
 * @param[out] status         Error status variable. 0 on success.
 * @return the mask of interrupts that fired
 */
int64_t HAL_WaitForMultipleInterrupts(HAL_InterruptHandle interruptHandle,
                                      int64_t mask, double timeout,
                                      HAL_Bool ignorePrevious, int32_t* status);

/**
 * Returns the timestamp for the rising interrupt that occurred most recently.
 *
 * This is in the same time domain as HAL_GetFPGATime().  It only contains the
 * bottom 32 bits of the timestamp.  If your robot has been running for over 1
 * hour, you will need to fill in the upper 32 bits yourself.
 *
 * @param[in] interruptHandle the interrupt handle
 * @param[out] status         Error status variable. 0 on success.
 * @return timestamp in microseconds since FPGA Initialization
 */
int64_t HAL_ReadInterruptRisingTimestamp(HAL_InterruptHandle interruptHandle,
                                         int32_t* status);

/**
 * Returns the timestamp for the falling interrupt that occurred most recently.
 *
 * This is in the same time domain as HAL_GetFPGATime().  It only contains the
 * bottom 32 bits of the timestamp.  If your robot has been running for over 1
 * hour, you will need to fill in the upper 32 bits yourself.
 *
 * @param[in] interruptHandle the interrupt handle
 * @param[out] status         Error status variable. 0 on success.
 * @return timestamp in microseconds since FPGA Initialization
 */
int64_t HAL_ReadInterruptFallingTimestamp(HAL_InterruptHandle interruptHandle,
                                          int32_t* status);

/**
 * Requests interrupts on a specific digital source.
 *
 * @param[in] interruptHandle     the interrupt handle
 * @param[in] digitalSourceHandle the digital source handle (either a
 *                                HAL_AnalogTriggerHandle or a
 *                                HAL_DigitalHandle)
 * @param[in] analogTriggerType   the trigger type if the source is an
 *                                AnalogTrigger
 * @param[out] status             Error status variable. 0 on success.
 */
void HAL_RequestInterrupts(HAL_InterruptHandle interruptHandle,
                           HAL_Handle digitalSourceHandle,
                           HAL_AnalogTriggerType analogTriggerType,
                           int32_t* status);

/**
 * Sets the edges to trigger the interrupt on.
 *
 * Note that both edges triggered is a valid configuration.
 *
 * @param[in] interruptHandle the interrupt handle
 * @param[in] risingEdge      true for triggering on rising edge
 * @param[in] fallingEdge     true for triggering on falling edge
 * @param[out] status         Error status variable. 0 on success.
 */
void HAL_SetInterruptUpSourceEdge(HAL_InterruptHandle interruptHandle,
                                  HAL_Bool risingEdge, HAL_Bool fallingEdge,
                                  int32_t* status);

/**
 * Releases a waiting interrupt.
 *
 * This will release both rising and falling waiters.
 *
 * @param[in] interruptHandle the interrupt handle to release
 * @param[out] status         Error status variable. 0 on success.
 */
void HAL_ReleaseWaitingInterrupt(HAL_InterruptHandle interruptHandle,
                                 int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */

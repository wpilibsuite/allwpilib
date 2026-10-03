// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/Types.h"

/**
 * @defgroup hal_counter Counter Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a counter.
 *
 * @param[in] channel               the dio channel
 * @param[in] risingEdge            true to count on rising edge, false for
 * falling
 * @param[in] allocationLocation    the location where the allocation is
 * occurring (can be null)
 * @param[out] status     Error status variable. 0 on success.
 * @return the created handle
 */
HAL_CounterHandle HAL_InitializeCounter(int channel, HAL_Bool risingEdge,
                                        const char* allocationLocation,
                                        int32_t* status);

/**
 * Frees a counter.
 *
 * @param[in] counterHandle the counter handle
 */
void HAL_FreeCounter(HAL_CounterHandle counterHandle);

/**
 * Sets the counter to detect rising or falling edges.
 *
 * @param[in] counterHandle  the counter handle
 * @param[in] risingEdge     true to count rising edges, false to count falling
 * @param[out] status        Error status variable. 0 on success.
 */
void HAL_SetCounterEdgeConfiguration(HAL_CounterHandle counterHandle,
                                     HAL_Bool risingEdge, int32_t* status);

/**
 * Sets the time window used to calculate the counter rate.
 *
 * @param[in] counterHandle      the counter handle
 * @param[in] windowMilliseconds the rate calculation window in milliseconds;
 *                               valid values are 5 through 255
 * @param[out] status            Error status variable. 0 on success.
 */
void HAL_SetCounterRateWindow(HAL_CounterHandle counterHandle,
                              int32_t windowMilliseconds, int32_t* status);

/**
 * Resets the Counter to zero.
 *
 * Sets the counter value to zero. This does not effect the running state of the
 * counter, just sets the current value to zero.
 *
 * @param[in] counterHandle the counter handle
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_ResetCounter(HAL_CounterHandle counterHandle, int32_t* status);

/**
 * Reads the current counter value.
 *
 * Reads the value at this instant. It may still be running, so it reflects the
 * current value. Next time it is read, it might have a different value.
 *
 * @param[in] counterHandle the counter handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the current counter value
 */
int32_t HAL_GetCounter(HAL_CounterHandle counterHandle, int32_t* status);

/**
 * Gets the current counter rate.
 *
 * @param[in] counterHandle the counter handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the counter rate in counts per second
 */
double HAL_GetCounterRate(HAL_CounterHandle counterHandle, int32_t* status);

/**
 * Determines if the clock is stopped.
 *
 * Determines if the counter's current rate is zero.
 *
 * @param[in] counterHandle the counter handle
 * @param[out] status       Error status variable. 0 on success.
 * @return true if the counter's current rate is zero
 */
HAL_Bool HAL_GetCounterStopped(HAL_CounterHandle counterHandle,
                               int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */

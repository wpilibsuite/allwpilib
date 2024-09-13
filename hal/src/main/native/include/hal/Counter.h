// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/AnalogTrigger.h"
#include "hal/Types.h"

/**
 * @defgroup hal_counter Counter Functions
 * @ingroup hal_capi
 * @{
 */

/**
 * The counter mode.
 */
HAL_ENUM(HAL_Counter_Mode) {
  HAL_Counter_kTwoPulse = 0,
  HAL_Counter_kSemiperiod = 1,
  HAL_Counter_kPulseLength = 2,
  HAL_Counter_kExternalDirection = 3
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a counter.
 *
 * @param[in] mode    the counter mode
 * @param[in] index   the compressor index (output)
 * @param[out] status Error status variable. 0 on success.
 * @return the created handle
 */
HAL_CounterHandle HAL_InitializeCounter(HAL_Counter_Mode mode, int32_t* index,
                                        int32_t* status);

/**
 * Frees a counter.
 *
 * @param[in] counterHandle the counter handle
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_FreeCounter(HAL_CounterHandle counterHandle, int32_t* status);

/**
 * Sets the average sample size of a counter.
 *
 * @param[in] counterHandle  the counter handle
 * @param[in] size           the size of samples to average
 * @param[out] status        Error status variable. 0 on success.
 */
void HAL_SetCounterAverageSize(HAL_CounterHandle counterHandle, int32_t size,
                               int32_t* status);

/**
 * Sets the source object that causes the counter to count up.
 *
 * @param[in] counterHandle       the counter handle
 * @param[in] digitalSourceHandle the digital source handle (either a
 *                                HAL_AnalogTriggerHandle or a
 *                                HAL_DigitalHandle)
 * @param[in] analogTriggerType   the analog trigger type if the source is an
 *                                analog trigger
 * @param[out] status             Error status variable. 0 on success.
 */
void HAL_SetCounterUpSource(HAL_CounterHandle counterHandle,
                            HAL_Handle digitalSourceHandle,
                            HAL_AnalogTriggerType analogTriggerType,
                            int32_t* status);

/**
 * Sets the up source to either detect rising edges or falling edges.
 *
 * Note that both are allowed to be set true at the same time without issues.
 *
 * @param[in] counterHandle  the counter handle
 * @param[in] risingEdge     true to trigger on rising
 * @param[in] fallingEdge    true to trigger on falling
 * @param[out] status        Error status variable. 0 on success.
 */
void HAL_SetCounterUpSourceEdge(HAL_CounterHandle counterHandle,
                                HAL_Bool risingEdge, HAL_Bool fallingEdge,
                                int32_t* status);

/**
 * Disables the up counting source to the counter.
 *
 * @param[in] counterHandle the counter handle
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_ClearCounterUpSource(HAL_CounterHandle counterHandle, int32_t* status);

/**
 * Sets the source object that causes the counter to count down.
 *
 * @param[in] counterHandle       the counter handle
 * @param[in] digitalSourceHandle the digital source handle (either a
 *                                HAL_AnalogTriggerHandle or a
 *                                HAL_DigitalHandle)
 * @param[in] analogTriggerType   the analog trigger type if the source is an
 *                                analog trigger
 * @param[out] status             Error status variable. 0 on success.
 */
void HAL_SetCounterDownSource(HAL_CounterHandle counterHandle,
                              HAL_Handle digitalSourceHandle,
                              HAL_AnalogTriggerType analogTriggerType,
                              int32_t* status);

/**
 * Sets the down source to either detect rising edges or falling edges.
 * Note that both are allowed to be set true at the same time without issues.
 *
 * @param[in] counterHandle  the counter handle
 * @param[in] risingEdge     true to trigger on rising
 * @param[in] fallingEdge    true to trigger on falling
 * @param[out] status        Error status variable. 0 on success.
 */
void HAL_SetCounterDownSourceEdge(HAL_CounterHandle counterHandle,
                                  HAL_Bool risingEdge, HAL_Bool fallingEdge,
                                  int32_t* status);

/**
 * Disables the down counting source to the counter.
 *
 * @param[in] counterHandle the counter handle
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_ClearCounterDownSource(HAL_CounterHandle counterHandle,
                                int32_t* status);

/**
 * Sets standard up / down counting mode on this counter.
 *
 * Up and down counts are sourced independently from two inputs.
 *
 * @param[in] counterHandle the counter handle
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetCounterUpDownMode(HAL_CounterHandle counterHandle, int32_t* status);

/**
 * Sets directional counting mode on this counter.
 *
 * The direction is determined by the B input, with counting happening with the
 * A input.
 *
 * @param[in] counterHandle the counter handle
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetCounterExternalDirectionMode(HAL_CounterHandle counterHandle,
                                         int32_t* status);

/**
 * Sets Semi-period mode on this counter.
 *
 * The counter counts up based on the time the input is triggered. High or Low
 * depends on the highSemiPeriod parameter.
 *
 * @param[in] counterHandle  the counter handle
 * @param[in] highSemiPeriod true for counting when the input is high, false for
 *                           low
 * @param[out] status        Error status variable. 0 on success.
 */
void HAL_SetCounterSemiPeriodMode(HAL_CounterHandle counterHandle,
                                  HAL_Bool highSemiPeriod, int32_t* status);

/**
 * Configures the counter to count in up or down based on the length of the
 * input pulse.
 *
 * This mode is most useful for direction sensitive gear tooth sensors.
 *
 * @param[in] counterHandle the counter handle
 * @param[in] threshold     The pulse length beyond which the counter counts the
 *                          opposite direction (seconds)
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetCounterPulseLengthMode(HAL_CounterHandle counterHandle,
                                   double threshold, int32_t* status);

/**
 * Gets the Samples to Average which specifies the number of samples of the
 * timer to average when calculating the period. Perform averaging to account
 * for mechanical imperfections or as oversampling to increase resolution.
 *
 * @param[in] counterHandle the counter handle
 * @param[out] status       Error status variable. 0 on success.
 * @return SamplesToAverage The number of samples being averaged (from 1 to 127)
 */
int32_t HAL_GetCounterSamplesToAverage(HAL_CounterHandle counterHandle,
                                       int32_t* status);

/**
 * Sets the Samples to Average which specifies the number of samples of the
 * timer to average when calculating the period. Perform averaging to account
 * for mechanical imperfections or as oversampling to increase resolution.
 *
 * @param[in] counterHandle    the counter handle
 * @param[in] samplesToAverage The number of samples to average from 1 to 127
 * @param[out] status          Error status variable. 0 on success.
 */
void HAL_SetCounterSamplesToAverage(HAL_CounterHandle counterHandle,
                                    int32_t samplesToAverage, int32_t* status);

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
 * Gets the Period of the most recent count.
 *
 * Returns the time interval of the most recent count. This can be used for
 * velocity calculations to determine shaft speed.
 *
 * @param[in] counterHandle the counter handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the period of the last two pulses in units of seconds
 */
double HAL_GetCounterPeriod(HAL_CounterHandle counterHandle, int32_t* status);

/**
 * Sets the maximum period where the device is still considered "moving".
 *
 * Sets the maximum period where the device is considered moving. This value is
 * used to determine the "stopped" state of the counter using the
 * HAL_GetCounterStopped method.
 *
 * @param[in] counterHandle the counter handle
 * @param[in] maxPeriod     the maximum period where the counted device is
 *                          considered moving in seconds
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetCounterMaxPeriod(HAL_CounterHandle counterHandle, double maxPeriod,
                             int32_t* status);

/**
 * Selects whether you want to continue updating the event timer output when
 * there are no samples captured.
 *
 * The output of the event timer has a buffer of periods that are averaged and
 * posted to a register on the FPGA.  When the timer detects that the event
 * source has stopped (based on the MaxPeriod) the buffer of samples to be
 * averaged is emptied.
 *
 * If you enable the update when empty, you will be
 * notified of the stopped source and the event time will report 0 samples.
 *
 * If you disable update when empty, the most recent average will remain on the
 * output until a new sample is acquired.
 *
 * You will never see 0 samples output (except when there have been no events
 * since an FPGA reset) and you will likely not see the stopped bit become true
 * (since it is updated at the end of an average and there are no samples to
 * average).
 *
 * @param[in] counterHandle the counter handle
 * @param[in] enabled       true to enable counter updating with no samples
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetCounterUpdateWhenEmpty(HAL_CounterHandle counterHandle,
                                   HAL_Bool enabled, int32_t* status);

/**
 * Determines if the clock is stopped.
 *
 * Determine if the clocked input is stopped based on the MaxPeriod value set
 * using the SetMaxPeriod method. If the clock exceeds the MaxPeriod, then the
 * device (and counter) are assumed to be stopped and it returns true.
 *
 * @param[in] counterHandle the counter handle
 * @param[out] status       Error status variable. 0 on success.
 * @return true if the most recent counter period exceeds the MaxPeriod value
 *         set by SetMaxPeriod
 */
HAL_Bool HAL_GetCounterStopped(HAL_CounterHandle counterHandle,
                               int32_t* status);

/**
 * Gets the last direction the counter value changed.
 *
 * @param[in] counterHandle the counter handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the last direction the counter value changed
 */
HAL_Bool HAL_GetCounterDirection(HAL_CounterHandle counterHandle,
                                 int32_t* status);

/**
 * Sets the Counter to return reversed sensing on the direction.
 *
 * This allows counters to change the direction they are counting in the case of
 * 1X and 2X quadrature encoding only. Any other counter mode isn't supported.
 *
 * @param[in] counterHandle    the counter handle
 * @param[in] reverseDirection true if the value counted should be negated.
 * @param[out] status          Error status variable. 0 on success.
 */
void HAL_SetCounterReverseDirection(HAL_CounterHandle counterHandle,
                                    HAL_Bool reverseDirection, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */

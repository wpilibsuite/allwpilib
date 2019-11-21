/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "hal/AnalogTrigger.h"
#include "hal/Types.h"

/**
 * @defgroup hal_dutycycle DutyCycle Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize a DutyCycle input.
 *
 * @param digitalSourceHandle the digital source to use (either a
 * HAL_DigitalHandle or a HAL_AnalogTriggerHandle)
 * @param triggerType the analog trigger type of the source if it is
 * an analog trigger
 * @return thre created duty cycle handle
 */
HAL_DutyCycleHandle HAL_InitializeDutyCycle(HAL_Handle digitalSourceHandle,
                                            HAL_AnalogTriggerType triggerType,
                                            int32_t* status);

/**
 * Free a DutyCycle.
 *
 * @param dutyCycleHandle the duty cycle handle
 */
void HAL_FreeDutyCycle(HAL_DutyCycleHandle dutyCycleHandle);

/**
 * Indicates the duty cycle is used by a simulated device.
 *
 * @param handle the duty cycle handle
 * @param device simulated device handle
 */
void HAL_SetDutyCycleSimDevice(HAL_DutyCycleHandle handle,
                               HAL_SimDeviceHandle device);

/**
 * Get the frequency of the duty cycle signal.
 *
 * @param dutyCycleHandle the duty cycle handle
 * @return frequency in Hertz
 */
int32_t HAL_GetDutyCycleFrequency(HAL_DutyCycleHandle dutyCycleHandle,
                                  int32_t* status);

/**
 * Get the output ratio of the duty cycle signal.
 *
 * <p> 0 means always low, 1 means always high.
 *
 * @param dutyCycleHandle the duty cycle handle
 * @return output ratio between 0 and 1
 */
double HAL_GetDutyCycleOutput(HAL_DutyCycleHandle dutyCycleHandle,
                              int32_t* status);

/**
 * Get the raw output ratio of the duty cycle signal.
 *
 * <p> 0 means always low, an output equal to
 * GetOutputScaleFactor() means always high.
 *
 * @param dutyCycleHandle the duty cycle handle
 * @return output ratio in raw units
 */
int32_t HAL_GetDutyCycleOutputRaw(HAL_DutyCycleHandle dutyCycleHandle,
                                  int32_t* status);

/**
 * Get the scale factor of the output.
 *
 * <p> An output equal to this value is always high, and then linearly scales
 * down to 0. Divide the result of getOutputRaw by this in order to get the
 * percentage between 0 and 1.
 *
 * @param dutyCycleHandle the duty cycle handle
 * @return the output scale factor
 */
int32_t HAL_GetDutyCycleOutputScaleFactor(HAL_DutyCycleHandle dutyCycleHandle,
                                          int32_t* status);

/**
 * Get the FPGA index for the DutyCycle.
 *
 * @param dutyCycleHandle the duty cycle handle
 * @return the FPGA index
 */
int32_t HAL_GetDutyCycleFPGAIndex(HAL_DutyCycleHandle dutyCycleHandle,
                                  int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */

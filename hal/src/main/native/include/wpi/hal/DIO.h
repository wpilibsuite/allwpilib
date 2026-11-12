// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/Types.h"

/**
 * @defgroup hal_dio DIO Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates a new instance of a digital port.
 *
 * @param[in] channel            the smartio channel
 * @param[in] input              true for input, false for output
 * @param[in] allocationLocation the location where the allocation is occurring
 *                               (can be null)
 * @param[out] status            Error status variable. 0 on success.
 * @return the created digital handle
 */
HAL_DigitalHandle HAL_InitializeDIOPort(int32_t channel, HAL_Bool input,
                                        const char* allocationLocation,
                                        int32_t* status);

/**
 * Checks if a DIO channel is valid.
 *
 * @param channel the channel number to check
 * @return true if the channel is valid, otherwise false
 */
HAL_Bool HAL_CheckDIOChannel(int32_t channel);

/**
 * Frees a DIO port.
 *
 * @param dioPortHandle the DIO channel handle
 */
void HAL_FreeDIOPort(HAL_DigitalHandle dioPortHandle);

/**
 * Indicates the DIO channel is used by a simulated device.
 *
 * @param handle the DIO channel handle
 * @param device simulated device handle
 */
void HAL_SetDIOSimDevice(HAL_DigitalHandle handle, HAL_SimDeviceHandle device);

/**
 * Writes a digital value to a DIO channel.
 *
 * @param[in] dioPortHandle the digital port handle
 * @param[in] value         the state to set the digital channel (if it is
 *                          configured as an output)
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetDIO(HAL_DigitalHandle dioPortHandle, HAL_Bool value,
                int32_t* status);

/**
 * Sets the direction of a DIO channel.
 *
 * @param[in] dioPortHandle the digital port handle
 * @param[in] input         true to set input, false for output
 * @param[out] status       Error status variable. 0 on success.
 */
void HAL_SetDIODirection(HAL_DigitalHandle dioPortHandle, HAL_Bool input,
                         int32_t* status);

/**
 * Reads a digital value from a DIO channel.
 *
 * @param[in] dioPortHandle the digital port handle
 * @param[out] status       Error status variable. 0 on success.
 * @return the state of the specified channel
 */
HAL_Bool HAL_GetDIO(HAL_DigitalHandle dioPortHandle, int32_t* status);

/**
 * Reads the direction of a DIO channel.
 *
 * @param[in] dioPortHandle the digital port handle
 * @param[out] status       Error status variable. 0 on success.
 * @return true for input, false for output
 */
HAL_Bool HAL_GetDIODirection(HAL_DigitalHandle dioPortHandle, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */

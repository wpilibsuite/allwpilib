// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/AddressableLEDTypes.h"
#include "wpi/hal/Types.h"

/**
 * @defgroup hal_addressable Addressable LED Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates a new instance of an addressable LED.
 *
 * @param[in] channel            the smartio channel
 * @param[in] allocationLocation the location where the allocation is occurring
 *                               (can be null)
 * @param[out] status the error code, or 0 for success
 * @return Addressable LED handle
 */
HAL_AddressableLEDHandle HAL_InitializeAddressableLED(
    int32_t channel, const char* allocationLocation, int32_t* status);

/**
 * Free the Addressable LED Handle.
 *
 * @param[in] handle the Addressable LED handle to free
 */
void HAL_FreeAddressableLED(HAL_AddressableLEDHandle handle);

/**
 * Sets the start buffer location used for the LED strip.
 *
 * <p>All addressable LEDs use a single backing buffer 1024 LEDs in size.
 * The max length for a single output is 1024 LEDs (with an offset of zero).
 *
 * @param[in] handle the Addressable LED handle
 * @param[in] start the strip start, in LEDs
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetAddressableLEDStart(HAL_AddressableLEDHandle handle, int32_t start,
                                int32_t* status);

/**
 * Sets the length of the LED strip.
 *
 * <p>All addressable LEDs use a single backing buffer 1024 LEDs in size.
 * The max length for a single output is 1024 LEDs (with an offset of zero).
 *
 * @param[in] handle the Addressable LED handle
 * @param[in] length the strip length, in LEDs
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetAddressableLEDLength(HAL_AddressableLEDHandle handle,
                                 int32_t length, int32_t* status);

/**
 * Sets the led output data.
 *
 * <p>All addressable LEDs use a single backing buffer 1024 LEDs in size.
 * This function may be used to set part of or all of the buffer.
 *
 * @param[in] start the strip start, in LEDs
 * @param[in] length the strip length, in LEDs
 * @param[in] colorOrder the color order
 * @param[in] data the buffer to write
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetAddressableLEDData(int32_t start, int32_t length,
                               HAL_AddressableLEDColorOrder colorOrder,
                               const struct HAL_AddressableLEDData* data,
                               int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */

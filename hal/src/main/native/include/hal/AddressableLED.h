// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/AddressableLEDTypes.h"
#include "hal/Types.h"

/**
 * @defgroup hal_addressable Addressable LED Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize Addressable LED using a PWM Digital handle.
 *
 * @param[in] outputPort handle of the digital port for PWM
 * @param[out] status the error code, or 0 for success
 * @return Addressable LED handle
 */
HAL_AddressableLEDHandle HAL_InitializeAddressableLED(
    HAL_DigitalHandle outputPort, int32_t* status);

/**
 * Free the Addressable LED Handle.
 *
 * @param[in] handle the Addressable LED handle to free
 */
void HAL_FreeAddressableLED(HAL_AddressableLEDHandle handle);

/**
 * Sets the color order for the addressable LED output. The default order is
 * GRB. This will take effect on the next call to HAL_WriteAddressableLEDData().
 * @param[in] handle the Addressable LED handle
 * @param[in] colorOrder the color order
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetAddressableLEDColorOrder(HAL_AddressableLEDHandle handle,
                                     HAL_AddressableLEDColorOrder colorOrder,
                                     int32_t* status);

/**
 * Set the Addressable LED PWM Digital port.
 *
 * @param[in] handle the Addressable LED handle
 * @param[in] outputPort The digital handle of the PWM port
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetAddressableLEDOutputPort(HAL_AddressableLEDHandle handle,
                                     HAL_DigitalHandle outputPort,
                                     int32_t* status);

/**
 * Sets the length of the LED strip.
 *
 * <p>The max length is 5460 LEDs.
 *
 * @param[in] handle the Addressable LED handle
 * @param[in] length the strip length
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetAddressableLEDLength(HAL_AddressableLEDHandle handle,
                                 int32_t length, int32_t* status);

/**
 * Sets the led output data.
 *
 * <p>If the output is enabled, this will start writing the next data cycle.
 * It is safe to call, even while output is enabled.
 *
 * @param[in] handle the Addressable LED handle
 * @param[in] data the buffer to write
 * @param[in] length the strip length
 * @param[out] status the error code, or 0 for success
 */
void HAL_WriteAddressableLEDData(HAL_AddressableLEDHandle handle,
                                 const struct HAL_AddressableLEDData* data,
                                 int32_t length, int32_t* status);

/**
 * Sets the bit timing.
 *
 * <p>By default, the driver is set up to drive WS2812B and WS2815, so nothing
 * needs to be set for those.
 *
 * @param[in] handle the Addressable LED handle
 * @param[in] highTime0NanoSeconds high time for 0 bit (default 400ns)
 * @param[in] lowTime0NanoSeconds low time for 0 bit (default 900ns)
 * @param[in] highTime1NanoSeconds high time for 1 bit (default 900ns)
 * @param[in] lowTime1NanoSeconds low time for 1 bit (default 600ns)
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetAddressableLEDBitTiming(HAL_AddressableLEDHandle handle,
                                    int32_t highTime0NanoSeconds,
                                    int32_t lowTime0NanoSeconds,
                                    int32_t highTime1NanoSeconds,
                                    int32_t lowTime1NanoSeconds,
                                    int32_t* status);

/**
 * Sets the sync time.
 *
 * <p>The sync time is the time to hold output so LEDs enable. Default set for
 * WS2812B and WS2815.
 *
 * @param[in] handle the Addressable LED handle
 * @param[in] syncTimeMicroSeconds the sync time (default 280us)
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetAddressableLEDSyncTime(HAL_AddressableLEDHandle handle,
                                   int32_t syncTimeMicroSeconds,
                                   int32_t* status);

/**
 * Starts the output.
 *
 * <p>The output writes continuously.
 *
 * @param[in] handle the Addressable LED handle
 * @param[out] status the error code, or 0 for success
 */
void HAL_StartAddressableLEDOutput(HAL_AddressableLEDHandle handle,
                                   int32_t* status);

/**
 * Stops the output.
 *
 * @param[in] handle the Addressable LED handle
 * @param[out] status the error code, or 0 for success
 */
void HAL_StopAddressableLEDOutput(HAL_AddressableLEDHandle handle,
                                  int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */

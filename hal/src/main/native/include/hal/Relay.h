/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_relay Relay Output Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a relay.
 *
 * Note this call will only initialize either the forward or reverse port of the
 * relay. If you need both, you will need to initialize 2 relays.
 *
 * @param portHandle the port handle to initialize
 * @param fwd        true for the forward port, false for the reverse port
 * @return           the created relay handle
 */
HAL_RelayHandle HAL_InitializeRelayPort(HAL_PortHandle portHandle, HAL_Bool fwd,
                                        int32_t* status);

/**
 * Frees a relay port.
 *
 * @param relayPortHandle the relay handle
 */
void HAL_FreeRelayPort(HAL_RelayHandle relayPortHandle);

/**
 * Checks if a relay channel is valid.
 *
 * @param channel the channel to check
 * @return        true if the channel is valid, otherwise false
 */
HAL_Bool HAL_CheckRelayChannel(int32_t channel);

/**
 * Sets the state of a relay output.
 *
 * @param relayPortHandle the relay handle
 * @param on              true for on, false for off
 */
void HAL_SetRelay(HAL_RelayHandle relayPortHandle, HAL_Bool on,
                  int32_t* status);

/**
 * Gets the current state of the relay channel.
 *
 * @param relayPortHandle the relay handle
 * @return                true for on, false for off
 */
HAL_Bool HAL_GetRelay(HAL_RelayHandle relayPortHandle, int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
